
#include <gtest/gtest.h>
#include <string>

std::string getSalutation()
{
    return "Hello, World!";
}

// Test Case
TEST(ClientTestTest, Static)
{
    EXPECT_EQ(getSalutation(), "Hello, World!"); // Compare the output of getSalutation
}