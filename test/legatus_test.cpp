#include "legatus.h"

#include "gtest/gtest.h"

class LegatusTest : public ::testing::Test {
  protected:
    legatus::Legatus legatus{};
};

TEST_F(LegatusTest, Adder) {
    legatus.add(1, 2);
    ASSERT_EQ(legatus.get_result(), 3);
}

TEST_F(LegatusTest, Version) {
    ASSERT_NE(legatus::Legatus::get_version(), "");
}
