
#include <shared/message_types.h>
#include "gtest/gtest.h"

using namespace shared;

TEST(SharedLibraryTest, StartGameRequestMessageConstructorSuccess)
{
    std::vector<std::string> selected_cards = {"Adventurer", "CouncilRoom", "Feast",   "Gardens", "Mine",
                                               "Remodel",    "Smithy",      "Village", "Baron",   "GreatHall"};
    StartGameRequestMessage msg("game_id", "message_id", "player_id", selected_cards);
}

TEST(SharedLibraryTest, StartGameRequestMessageConstructorFailure)
{
    std::vector<std::string> selected_cards = {"Adventurer", "CouncilRoom", "Feast",   "Gardens", "Mine",
                                               "Remodel",    "Smithy",      "Village", "Baron"};
    EXPECT_DEATH(StartGameRequestMessage("game_id", "message_id", "player_id", selected_cards), "Assertion failed");
}
