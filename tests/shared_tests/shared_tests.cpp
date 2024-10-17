
#include <gtest/gtest.h>
#include <string>

std::string getSalutation()
{
    return "Hello, World!";
}

// Test Case
TEST(SharedTestTest, Dynamic)
{
    EXPECT_EQ(getSalutation(), "Hello, World!"); // Compare the output of getSalutation
}