#pragma once

#include <sys/event.h>

#include <cstdint>

#include <functional>
#include <unordered_map>

#include "legatus/status.h"

namespace legatus::io {

using TimerCallback = std::function<void(uint64_t, int64_t)>;

class EventLoop {
  public:
    EventLoop();
    EventLoop(const EventLoop&) = delete;
    EventLoop& operator=(const EventLoop&) = delete;
    EventLoop(EventLoop&&) = delete;
    EventLoop& operator=(EventLoop&&) = delete;

    ~EventLoop();

    Status<None, int> shutdown() const;

    Status<None, int> register_timer(uint64_t id,
                                     uint64_t timeout,
                                     bool periodic,
                                     const TimerCallback& cb);

    void run();

  private:
    static constexpr uint64_t k_shutdown_event_id = 19;

    int kq_;
    bool done_ = false;
    std::unordered_map<uint64_t, TimerCallback> timers_;

    void do_shutdown();
};

} // namespace legatus::io
