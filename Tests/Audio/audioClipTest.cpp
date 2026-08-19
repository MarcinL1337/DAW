#include <gtest/gtest.h>

TEST(DummyTestSuite, DummyTestForJulian)
{
    constexpr auto dummyValue{1u};
    ASSERT_EQ(dummyValue, 1);
    ASSERT_NE(dummyValue, 1337);
    ASSERT_GE(dummyValue, 1);
}