#pragma once

#include <cstdint>
#include <functional>
#include <sys/event.h>
#include <unordered_map>

namespace legatus::io {

using TimerCallback = std::function<void(uintptr_t, intptr_t)>;

class EventLoop {
  public:
    EventLoop();

    void shutdown() const;

    int register_timer(int id, uint64_t timeout, bool periodic, const TimerCallback& cb);

    void run();

  private:
    static constexpr uintptr_t k_shutdown_event_id = 19;
    int kq_;
    bool done_ = false;
    std::unordered_map<uintptr_t, TimerCallback> timers_;

    void do_shutdown();
};

} // namespace legatus::io
