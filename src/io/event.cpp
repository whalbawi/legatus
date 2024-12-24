#include "legatus/io/event.h"

#include <cstdint>
#include <cstdio>
#include <functional>
#include <sys/event.h>
#include <vector>

namespace legatus::io {

EventLoop::EventLoop() : kq_(kqueue()) {
    if (kq_ == -1) {
        throw std::exception();
    }

    // Register shutdown handler
    struct kevent ev{};
    EV_SET(&ev, k_shutdown_event_id, EVFILT_USER, EV_ADD, 0, 0, nullptr);
    kevent(kq_, &ev, 1, nullptr, 0, nullptr);
}

int EventLoop::register_timer(int id, uint64_t timeout, bool periodic, const TimerCallback& cb) {
    struct kevent ev{};
    const uint16_t oneshot = periodic ? 0 : EV_ONESHOT;

    EV_SET(&ev, id, EVFILT_TIMER, EV_ADD | oneshot, NOTE_NSECONDS, timeout, nullptr);
    timers_[id] = cb;

    return kevent(kq_, &ev, 1, nullptr, 0, nullptr);
}

void EventLoop::shutdown() const {
    std::vector<struct kevent> evs;

    const uint32_t evs_cnt = timers_.size() + 1;

    for (const auto& timer : timers_) {
        struct kevent ev{};
        EV_SET(&ev, timer.first, EVFILT_TIMER, EV_DELETE, 0, 0, nullptr);
        evs.emplace_back(ev);
    }

    struct kevent ev{};
    EV_SET(&ev, k_shutdown_event_id, EVFILT_USER, 0, NOTE_TRIGGER, 0, nullptr);
    evs.emplace_back(ev);

    kevent(kq_, evs.data(), static_cast<int>(evs.size()), nullptr, 0, nullptr);
}

void EventLoop::run() {
    struct kevent ev{};

    while (!done_) {
        const int ret = kevent(kq_, nullptr, 0, &ev, 1, nullptr);
        if (ret == -1) {
            perror("failed to wait for events");
            continue;
        }

        const uintptr_t id = ev.ident;

        switch (ev.filter) {
        case EVFILT_USER:
            if (id == k_shutdown_event_id) {
                done_ = true;
            }
            break;

        case EVFILT_TIMER:
            const auto& cb = timers_[id];
            if ((ev.flags & EV_ERROR) != 0) {
                cb(ev.ident, ev.data);
            } else {
                cb(ev.ident, 0);
            }
        }
    }
}

} // namespace legatus::io
