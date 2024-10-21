
#include <gtest/gtest.h>
#include <string>

std::string getSalutation()
{
    return "Hello, Server Test!";
}

// Test Case
TEST(ServerTestTest, Static)
{
    EXPECT_EQ(getSalutation(), "Hello, Server Test!"); // Compare the output of getSalutation
}