#include "legatus.h"

#include "gtest/gtest.h"

class LegatusTest : public ::testing::Test {
  protected:
    legatus::Legatus legatus_{};
};

TEST_F(LegatusTest, Adder) {
    legatus_.add(1, 2);
    ASSERT_EQ(legatus_.get_result(), 3);
}

TEST_F(LegatusTest, Version) {
    ASSERT_NE(legatus::Legatus::get_version(), "");
}
