#include <client/example.h>
#include "gtest/gtest.h"

TEST(ClientTests, ExampleTest1) { EXPECT_EQ(client::test(), true); }
TEST(ClientTests, ExampleTest2) { EXPECT_EQ(rapidjson::test_rapidjson(), true); }
