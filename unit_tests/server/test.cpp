#include <server/example.h>
#include "gtest/gtest.h"

TEST(ServerTests, ExampleTest1) { EXPECT_EQ(server::test(), 1); }