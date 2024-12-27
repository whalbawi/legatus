// NOLINTBEGIN(readability-function-cognitive-complexity)

#include "legatus/io/event.h"

#include <cstdint>

#include "gtest/gtest.h"

namespace legatus::io {
TEST(EventLoopTest, TimerOneshot) {
    legatus::io::EventLoop ev_loop;
    int timer_id = 1;
    const uint64_t delay = 5e8;
    bool called = false;

    const TimerCallback cb = [&](uint64_t id, int64_t err) {
        ASSERT_EQ(timer_id, id);
        ASSERT_EQ(0, err);

        called = true;

        ev_loop.shutdown();
    };

    const int res = ev_loop.register_timer(timer_id, delay, false /* periodic */, cb);
    ASSERT_EQ(0, res);

    ev_loop.run();

    ASSERT_EQ(true, called);
}

TEST(EventLoopTest, TimerPeriodic) {
    legatus::io::EventLoop ev_loop;
    int timer_id = 1;
    const uint64_t delay = 5e8;
    int counter = 0;
    int counter_max = 4;

    const TimerCallback cb = [&](uint64_t id, int64_t err) {
        ASSERT_EQ(timer_id, id);
        ASSERT_EQ(0, err);

        ++counter;

        if (counter == counter_max) {
            ev_loop.shutdown();
        }
    };

    const int res = ev_loop.register_timer(timer_id, delay, true /* periodic */, cb);
    ASSERT_EQ(0, res);

    ev_loop.run();

    ASSERT_EQ(counter_max, counter);
}

TEST(EventLoopTest, TimerUpdate) {
    legatus::io::EventLoop ev_loop;
    int timer_id = 1;
    const uint64_t delay = 5e8;
    int counter = 0;
    int counter_max = 4;
    bool called = false;

    const TimerCallback cb_oneshot = [&](uint64_t id, int64_t err) {
        ASSERT_EQ(timer_id, id);
        ASSERT_EQ(0, err);

        called = true;

        ev_loop.shutdown();
    };

    const TimerCallback cb_periodic = [&](uint64_t id, int64_t err) {
        ASSERT_EQ(timer_id, id);
        ASSERT_EQ(0, err);
        ++counter;
        if (counter == counter_max) {
            const int res = ev_loop.register_timer(id, delay, false /* periodic */, cb_oneshot);
            ASSERT_EQ(0, res);
        }
    };

    const int res = ev_loop.register_timer(timer_id, delay, true /* periodic */, cb_periodic);
    ASSERT_EQ(0, res);

    ev_loop.run();

    ASSERT_EQ(counter_max, counter);
}

} // namespace legatus::io
// NOLINTEND(readability-function-cognitive-complexity)
