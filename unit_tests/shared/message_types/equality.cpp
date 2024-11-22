
#include <shared/message_types.h>
#include "gtest/gtest.h"

using namespace shared;

// ======= SERVER -> CLIENT ======= //

TEST(SharedLibraryTest, GameStateRequestMessageEquality)
{
    GameStateRequestMessage message1("game1", "player1", "message1");
    ASSERT_EQ(message1, message1);

    GameStateRequestMessage message2("game1", "player1", "message1");
    ASSERT_EQ(message1, message2);

    GameStateRequestMessage message3("game2", "player1", "message1");
    ASSERT_NE(message1, message3);

    GameStateRequestMessage message4("game1", "player1", "message2");
    ASSERT_NE(message1, message4);

    GameStateRequestMessage message5("game1", "player2", "message1");
    ASSERT_NE(message1, message5);
}

TEST(SharedLibraryTest, CreateLobbyRequestMessageEquality)
{
    CreateLobbyRequestMessage message1("game1", "player1", "message1");
    ASSERT_EQ(message1, message1);

    CreateLobbyRequestMessage message2("game1", "player1", "message1");
    ASSERT_EQ(message1, message2);

    CreateLobbyRequestMessage message3("game2", "player1", "message1");
    ASSERT_NE(message1, message3);

    CreateLobbyRequestMessage message4("game1", "player1", "message2");
    ASSERT_NE(message1, message4);

    CreateLobbyRequestMessage message5("game1", "player2", "message1");
    ASSERT_NE(message1, message5);
}

TEST(SharedLibraryTest, JoinLobbyRequestMessageEquality)
{
    JoinLobbyRequestMessage message1("game1", "player1", "message1");
    ASSERT_EQ(message1, message1);

    JoinLobbyRequestMessage message2("game1", "player1", "message1");
    ASSERT_EQ(message1, message2);

    JoinLobbyRequestMessage message3("game2", "player1", "message1");
    ASSERT_NE(message1, message3);

    JoinLobbyRequestMessage message4("game1", "player1", "message2");
    ASSERT_NE(message1, message4);

    JoinLobbyRequestMessage message5("game1", "player2", "message1");
    ASSERT_NE(message1, message5);
}

TEST(SharedLibraryTest, StartGameRequestMessageEquality)
{
    std::vector<CardBase::id_t> selected_cards = {"Adventurer", "Bureaucrat", "Cellar",     "Chapel", "CouncilRoom",
                                                  "Festival",   "Gardens",    "Laboratory", "Market", "Militia"};
    StartGameRequestMessage message1("game1", "player1", selected_cards, "message1");
    ASSERT_EQ(message1, message1);

    StartGameRequestMessage message2("game1", "player1", selected_cards, "message1");
    ASSERT_EQ(message1, message2);

    StartGameRequestMessage message3("game2", "player1", selected_cards, "message1");
    ASSERT_NE(message1, message3);

    StartGameRequestMessage message4("game1", "player1", selected_cards, "message2");
    ASSERT_NE(message1, message4);

    StartGameRequestMessage message5("game1", "player2", selected_cards, "message1");
    ASSERT_NE(message1, message5);

    std::vector<CardBase::id_t> selected_cards2 = {"Adventurer", "Bureaucrat", "Cellar",     "Chapel", "CouncilRoom",
                                                   "Festival",   "Gardens",    "Laboratory", "Market", "Moat"};
    StartGameRequestMessage message6("game1", "player1", selected_cards2, "message1");
    ASSERT_NE(message1, message6);
}

TEST(SharedLibraryTest, ActionDecisionMessage)
{
    std::unique_ptr<ActionDecision> decision1 = std::make_unique<PlayActionCardDecision>(3);
    ActionDecisionMessage message1("game1", "player1", std::move(decision1), "message0", "message1");
    ASSERT_EQ(message1, message1);

    std::unique_ptr<ActionDecision> decision2 = std::make_unique<PlayActionCardDecision>(3);
    ActionDecisionMessage message2("game1", "player1", std::move(decision2), "message0", "message1");
    ASSERT_EQ(message1, message2);

    std::unique_ptr<ActionDecision> decision3 = std::make_unique<PlayActionCardDecision>(4);
    ActionDecisionMessage message3("game2", "player1", std::move(decision3), "message0", "message1");
    ASSERT_NE(message1, message3);

    std::unique_ptr<ActionDecision> decision4 = std::make_unique<PlayActionCardDecision>(3);
    ActionDecisionMessage message4("game1", "player1", std::move(decision4), "message0", "message2");
    ASSERT_NE(message1, message4);

    std::unique_ptr<ActionDecision> decision5 = std::make_unique<PlayActionCardDecision>(3);
    ActionDecisionMessage message5("game1", "player2", std::move(decision5), "message0", "message1");
    ASSERT_NE(message1, message5);

    std::unique_ptr<ActionDecision> decision6 = std::make_unique<BuyCardDecision>("Copper");
    ActionDecisionMessage message6("game1", "player1", std::move(decision6), "message0", "message1");
    ASSERT_NE(message1, message6);

    std::unique_ptr<ActionDecision> decision7 = std::make_unique<PlayActionCardDecision>(3);
    ActionDecisionMessage message7("game1", "player1", std::move(decision7), "", "message1");
    ASSERT_NE(message1, message7);
}

// ======= CLIENT -> SERVER ======= //

/* TODO: reenable
TEST(SharedLibraryTest, GameStateMessageEquality)
{
    // TODO: Implement game state
    ReducedGameState *game_state_p1 = nullptr;
    ASSERT_NE(game_state_p1, nullptr);
    ReducedGameState game_state1 = *game_state_p1;
    GameStateMessage message1("game1", "message1", game_state1, "message0");
    ASSERT_EQ(message1, message1);

    GameStateMessage message2("game1", "message1", game_state1, "message0");
    ASSERT_EQ(message1, message2);

    GameStateMessage message3("game2", "message1", game_state1, "message0");
    ASSERT_NE(message1, message3);

    GameStateMessage message4("game1", "message2", game_state1, "message0");
    ASSERT_NE(message1, message4);

    // TODO: Implement game state
    ReducedGameState *game_state_p2 = nullptr;
    ASSERT_NE(game_state_p2, nullptr);
    ReducedGameState game_state2 = *game_state_p2;
    GameStateMessage message5("game1", "message1", game_state2, "message0");
    ASSERT_NE(message1, message5);

    GameStateMessage message6("game1", "message1", game_state1);
    ASSERT_NE(message1, message6);
}
*/

TEST(SharedLibraryTest, CreateLobbyResponseMessageEquality)
{
    std::vector<CardBase::id_t> available_cards = {"Adventurer", "Bureaucrat",  "Cellar",     "Chapel", "CouncilRoom",
                                                   "Festival",   "Gardens",     "Laboratory", "Market", "Militia",
                                                   "Moat",       "Moneylender", "Remodel",    "Smithy", "Village",
                                                   "Witch",      "Woodcutter",  "Workshop"};
    CreateLobbyResponseMessage message1("game1", available_cards, "message0", "message1");
    ASSERT_EQ(message1, message1);

    CreateLobbyResponseMessage message2("game1", available_cards, "message0", "message1");
    ASSERT_EQ(message1, message2);

    CreateLobbyResponseMessage message3("game2", available_cards, "message0", "message1");
    ASSERT_NE(message1, message3);

    CreateLobbyResponseMessage message4("game1", available_cards, "message0", "message2");
    ASSERT_NE(message1, message4);

    std::vector<CardBase::id_t> available_cards2 = {"Adventurer", "Bureaucrat",  "Cellar",
                                                    "Chapel",     "CouncilRoom", "Festival"};
    CreateLobbyResponseMessage message5("game1", available_cards2, "message0", "message1");
    ASSERT_NE(message1, message5);

    CreateLobbyResponseMessage message6("game1", available_cards, "", "message1");
    ASSERT_NE(message1, message6);
}

TEST(SharedLibraryTest, JoinLobbyBroadcastMessageEquality)
{
    JoinLobbyBroadcastMessage message1("game1", "player1", "message1");
    ASSERT_EQ(message1, message1);

    JoinLobbyBroadcastMessage message2("game1", "player1", "message1");
    ASSERT_EQ(message1, message2);

    JoinLobbyBroadcastMessage message3("game2", "player1", "message1");
    ASSERT_NE(message1, message3);

    JoinLobbyBroadcastMessage message4("game1", "player1", "message2");
    ASSERT_NE(message1, message4);

    JoinLobbyBroadcastMessage message5("game1", "player2", "message1");
    ASSERT_NE(message1, message5);
}

TEST(SharedLibraryTest, StartGameBroadcastMessageEquality)
{
    StartGameBroadcastMessage message1("game1", "message1");
    ASSERT_EQ(message1, message1);

    StartGameBroadcastMessage message2("game1", "message1");
    ASSERT_EQ(message1, message2);

    StartGameBroadcastMessage message3("game2", "message1");
    ASSERT_NE(message1, message3);

    StartGameBroadcastMessage message4("game1", "message2");
    ASSERT_NE(message1, message4);
}

TEST(SharedLibraryTest, EndGameBroadcastMessageEquality)
{
    EndGameBroadcastMessage message1("game1", "message1");
    ASSERT_EQ(message1, message1);

    EndGameBroadcastMessage message2("game1", "message1");
    ASSERT_EQ(message1, message2);

    EndGameBroadcastMessage message3("game2", "message1");
    ASSERT_NE(message1, message3);

    EndGameBroadcastMessage message4("game1", "message2");
    ASSERT_NE(message1, message4);
}

TEST(SharedLibraryTest, ResultResponseMessageEquality)
{
    bool success = false;
    ResultResponseMessage message1("game1", success, "message0", "failed because of reasons", "message1");
    ASSERT_EQ(message1, message1);

    ResultResponseMessage message2("game1", success, "message0", "failed because of reasons", "message1");
    ASSERT_EQ(message1, message2);

    ResultResponseMessage message3("game2", success, "message0", "failed because of reasons", "message1");
    ASSERT_NE(message1, message3);

    ResultResponseMessage message4("game1", success, "message0", "failed because of reasons", "message2");
    ASSERT_NE(message1, message4);

    ResultResponseMessage message5("game1", !success, "message0", "failed because of reasons", "message1");
    ASSERT_NE(message1, message5);

    ResultResponseMessage message6("game1", success, "message1", "failed because of reasons", "message1");
    ASSERT_NE(message1, message6);

    ResultResponseMessage message7("game1", success, "message0", "", "message1");
    ASSERT_NE(message1, message7);
}

TEST(SharedLibraryTest, ActionOrderMessageEquality)
{
    std::unique_ptr<ActionOrder> order1 = std::make_unique<ChooseNCardsFromHandOrder>(3);
    ActionOrderMessage message1("game1", std::move(order1), "description", "message1");
    ASSERT_EQ(message1, message1);

    std::unique_ptr<ActionOrder> order2 = std::make_unique<ChooseNCardsFromHandOrder>(3);
    ActionOrderMessage message2("game1", std::move(order2), "description", "message1");
    ASSERT_EQ(message1, message2);

    std::unique_ptr<ActionOrder> order3 = std::make_unique<ChooseNCardsFromHandOrder>(3);
    ActionOrderMessage message3("game2", std::move(order3), "description", "message1");
    ASSERT_NE(message1, message3);

    std::unique_ptr<ActionOrder> order4 = std::make_unique<ChooseNCardsFromHandOrder>(3);
    ActionOrderMessage message4("game1", std::move(order4), "description", "message2");
    ASSERT_NE(message1, message4);

    std::unique_ptr<ActionOrder> order5 = std::make_unique<ChooseNCardsFromHandOrder>(4);
    ActionOrderMessage message5("game1", std::move(order5), "description", "message1");
    ASSERT_NE(message1, message5);

    std::unique_ptr<ActionOrder> order6 = std::make_unique<ChooseNCardsFromHandOrder>(3);
    ActionOrderMessage message6("game1", std::move(order6), "message1");
    ASSERT_NE(message1, message6);

    std::unique_ptr<ActionOrder> order7 = std::make_unique<ChooseNCardsFromHandOrder>(3);
    ActionOrderMessage message7("game1", std::move(order7), "description0", "message1");
    ASSERT_NE(message1, message7);
}
