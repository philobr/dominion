#include "gtest/gtest.h"
#include <client/example.h>

TEST(ClientTests, ExampleTest1)
{
    EXPECT_EQ(client::test(), true);
}
