#include "axle.h"

#include "gtest/gtest.h"

namespace axle {

class AxleTest : public ::testing::Test {
  protected:
    Axle axle_{};
};

TEST_F(AxleTest, Adder) {
    axle_.add(1, 2);
    ASSERT_EQ(axle_.get_result(), 3);
}

TEST_F(AxleTest, Version) {
    ASSERT_NE(Axle::get_version(), "");
}

} // namespace axle
