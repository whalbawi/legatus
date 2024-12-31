#include "legatus/io/event.h"

#include <sys/event.h>
#include <unistd.h>

#include <cerrno>
#include <cstdint>
#include <cstdio>

#include <stdexcept>

#include "legatus/status.h"

namespace legatus::io {

EventLoop::EventLoop() : kq_(kqueue()) {
    if (kq_ == -1) {
        throw std::runtime_error("failed to initialize kqueue");
    }

    // Register shutdown handler
    struct kevent ev{};
    EV_SET(&ev, k_shutdown_event_id, EVFILT_USER, EV_ADD, 0, 0, nullptr);
    const int ret = kevent(kq_, &ev, 1, nullptr, 0, nullptr);
    if (ret == -1) {
        throw std::runtime_error("failed to register shutdown handler");
    }
}

EventLoop::~EventLoop() {
    const int ret = close(kq_);
    if (ret == -1) {
        perror("failed to close kqueue file descriptor");
    }
}

Status<None, int> EventLoop::register_timer(uint64_t id,
                                            uint64_t timeout,
                                            bool periodic,
                                            const TimerCallback& cb) {
    struct kevent ev{};
    const uint16_t oneshot = periodic ? 0 : EV_ONESHOT;

    EV_SET(&ev, id, EVFILT_TIMER, EV_ADD | oneshot, NOTE_NSECONDS, timeout, nullptr);
    timers_[id] = cb;

    const int ret = kevent(kq_, &ev, 1, nullptr, 0, nullptr);
    if (ret == -1) {
        perror("failed to register timer");

        return Status<None, int>::make_err(errno);
    };

    return Status<None, int>::make_ok();
}

Status<None, int> EventLoop::shutdown() const {
    struct kevent ev{};
    EV_SET(&ev, k_shutdown_event_id, EVFILT_USER, 0, NOTE_TRIGGER, 0, nullptr);

    const int ret = kevent(kq_, &ev, 1, nullptr, 0, nullptr);
    if (ret == -1) {
        perror("failed to schedule shutdown event");

        return Status<None, int>::make_err(errno);
    }

    return Status<None, int>::make_ok();
}

void EventLoop::run() {
    struct kevent ev{};

    while (!done_) {
        const int ret = kevent(kq_, nullptr, 0, &ev, 1, nullptr);
        if (ret == -1) {
            perror("failed to wait for events");
            continue;
        }

        const uint64_t id = ev.ident;

        switch (ev.filter) {
        case EVFILT_USER: {
            if (id == k_shutdown_event_id) {
                done_ = true;
            }
            break;
        }

        case EVFILT_TIMER: {
            const auto& cb = timers_[id];
            if ((ev.flags & EV_ERROR) != 0) {
                cb(ev.ident, ev.data);
            } else {
                cb(ev.ident, 0);
            }
            break;
        }

        default:
            perror("unknown event type");
        }
    }
}

} // namespace legatus::io
