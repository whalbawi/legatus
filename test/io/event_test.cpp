#include "legatus/io/event.h"

#include <cstdint>
#include <cstdlib>
#include <format>
#include <functional>
#include <iostream>

#include "gtest/gtest.h"

template <typename... Args>
void print(std::format_string<Args...> fmt, Args&&... args) {
    std::cout << std::format(fmt, std::forward<Args>(args)...);
}

TEST(EventLoopTest, Works) {
    legatus::io::EventLoop ev_loop;

    const std::function<void(uintptr_t, intptr_t)> cb1 = [&ev_loop](uintptr_t id, intptr_t err) {
        print("timer1 fired id={}, err={}\n", id, err);
        ev_loop.shutdown();
    };
    const std::function<void(uintptr_t, intptr_t)> cb2 = [](uintptr_t id, intptr_t err) {
        print("timer2 fired id=0x{:02X}, err={}\n", id, err);
    };

    const uint64_t delay = 1e9;
    ev_loop.register_timer(1, delay, true, cb1);
    ev_loop.register_timer(19, delay / 2, true, cb2);

    ev_loop.run();
}
