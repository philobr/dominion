
#include <gtest/gtest.h>
#include <shared/game_state.h>

using namespace shared;

TEST(SharedLibraryTest, PileFromJson)
{
    Pile expected("Witch", 10);
    std::string json = "{\"card\":\"Witch\",\"count\":10}";
    Pile *actual = Pile::from_json(json);
    ASSERT_NE(actual, nullptr);
    EXPECT_EQ(*actual, expected);
}

TEST(SharedLibraryTest, PileJson2WayConversion)
{
    Pile expected("Moat", 10);
    std::string json = expected.to_json();
    Pile *actual = Pile::from_json(json);
    ASSERT_NE(actual, nullptr);
    EXPECT_EQ(*actual, expected);
}
