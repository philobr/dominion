#include "gtest/gtest.h"
#include <server/example.h>

TEST(ServerTests, ExampleTest1)
{
    EXPECT_EQ(server::test(), 1);
}
