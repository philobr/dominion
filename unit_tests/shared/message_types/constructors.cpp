
#include <gtest/gtest.h>
#include <shared/message_types.h>

using namespace shared;

TEST(SharedLibraryTest, StartGameRequestMessageConstructorSuccess)
{
    std::vector<std::string> selected_cards = {"Adventurer", "CouncilRoom", "Feast",   "Gardens", "Mine",
                                               "Remodel",    "Smithy",      "Village", "Baron",   "GreatHall"};
    StartGameRequestMessage msg("game_id", "player_id", selected_cards, "message_id");
}

TEST(SharedLibraryTest, StartGameRequestMessageConstructorFailure)
{
    std::vector<std::string> selected_cards = {"Adventurer", "CouncilRoom", "Feast",   "Gardens", "Mine",
                                               "Remodel",    "Smithy",      "Village", "Baron"};
    EXPECT_DEATH(StartGameRequestMessage("game_id", "player_id", selected_cards, "message_id"), "Assertion failed");
}
