
#include "gtest/gtest.h"

#include <shared/message_types.h>

using namespace shared;

// ======= SERVER TO CLIENT MESSAGES ======= //

TEST(SharedLibraryTest, GameStateMessageTwoWayConversion)
{
    // TODO: Implement ReducedGameState
    ReducedGameState *game_state = nullptr;
    ASSERT_TRUE(game_state);
    GameStateMessage original_message("123", "456", *game_state, "789");

    std::string json = original_message.to_json();

    std::unique_ptr<ServerToClientMessage> base_message;
    base_message = ServerToClientMessage::from_json(json);

    std::unique_ptr<GameStateMessage> parsed_message(dynamic_cast<GameStateMessage *>(base_message.release()));

    ASSERT_NE(parsed_message, nullptr);

    ASSERT_EQ(parsed_message->game_id, "123");
    ASSERT_EQ(parsed_message->message_id, "456");
    ASSERT_TRUE(parsed_message->in_response_to);
    ASSERT_EQ(parsed_message->in_response_to.value(), "789");
}

TEST(SharedLibraryTest, CreateLobbyResponseMessageTwoWayConversion)
{
    std::vector<CardBase::id_t> available_cards = {"copper", "silver", "gold", "estate", "duchy", "province", "curse"};
    CreateLobbyResponseMessage original_message("123", "456", available_cards, std::nullopt);

    std::string json = original_message.to_json();

    std::unique_ptr<ServerToClientMessage> base_message;
    base_message = ServerToClientMessage::from_json(json);

    std::unique_ptr<CreateLobbyResponseMessage> parsed_message(
            dynamic_cast<CreateLobbyResponseMessage *>(base_message.release()));

    ASSERT_NE(parsed_message, nullptr);

    ASSERT_EQ(parsed_message->game_id, "123");
    ASSERT_EQ(parsed_message->message_id, "456");
    ASSERT_FALSE(parsed_message->in_response_to);
    ASSERT_EQ(parsed_message->available_cards,
              std::vector<std::string>({"copper", "silver", "gold", "estate", "duchy", "province", "curse"}));
}

TEST(SharedLibraryTest, JoinLobbyBroadcastMessageTwoWayConversion)
{
    JoinLobbyBroadcastMessage original_message("123", "456", "player1");

    std::string json = original_message.to_json();

    std::unique_ptr<ServerToClientMessage> base_message;
    base_message = ServerToClientMessage::from_json(json);

    std::unique_ptr<JoinLobbyBroadcastMessage> parsed_message(
            dynamic_cast<JoinLobbyBroadcastMessage *>(base_message.release()));

    ASSERT_NE(parsed_message, nullptr);

    ASSERT_EQ(parsed_message->game_id, "123");
    ASSERT_EQ(parsed_message->message_id, "456");
    ASSERT_EQ(parsed_message->player_id, "player1");
}

TEST(SharedLibraryTest, StartGameBroadcastMessageTwoWayConversion)
{
    StartGameBroadcastMessage original_message("123", "456");

    std::string json = original_message.to_json();

    std::unique_ptr<ServerToClientMessage> base_message;
    base_message = ServerToClientMessage::from_json(json);

    std::unique_ptr<StartGameBroadcastMessage> parsed_message(
            dynamic_cast<StartGameBroadcastMessage *>(base_message.release()));

    ASSERT_NE(parsed_message, nullptr);

    ASSERT_EQ(parsed_message->game_id, "123");
    ASSERT_EQ(parsed_message->message_id, "456");
}

TEST(SharedLibraryTest, EndGameBroadcastMessageTwoWayConversion)
{
    EndGameBroadcastMessage original_message("123", "456");

    std::string json = original_message.to_json();

    std::unique_ptr<ServerToClientMessage> base_message;
    base_message = ServerToClientMessage::from_json(json);

    std::unique_ptr<EndGameBroadcastMessage> parsed_message(
            dynamic_cast<EndGameBroadcastMessage *>(base_message.release()));

    ASSERT_NE(parsed_message, nullptr);

    ASSERT_EQ(parsed_message->game_id, "123");
    ASSERT_EQ(parsed_message->message_id, "456");
}

TEST(SharedLibraryTest, ResultResponseMessageTwoWayConversion)
{
    bool success = true;
    std::string in_response_to = "hui";
    std::string additional_information = "hey";
    ResultResponseMessage original_message("123", "456", success, in_response_to, additional_information);

    std::string json = original_message.to_json();

    std::unique_ptr<ServerToClientMessage> base_message;
    base_message = ServerToClientMessage::from_json(json);

    std::unique_ptr<ResultResponseMessage> parsed_message(
            dynamic_cast<ResultResponseMessage *>(base_message.release()));

    ASSERT_NE(parsed_message, nullptr);

    ASSERT_EQ(parsed_message->game_id, "123");
    ASSERT_EQ(parsed_message->message_id, "456");
    ASSERT_TRUE(parsed_message->in_response_to);
    ASSERT_EQ(parsed_message->in_response_to.value(), "hui");
    ASSERT_TRUE(parsed_message->success);
    ASSERT_EQ(parsed_message->success, true);
    ASSERT_TRUE(parsed_message->additional_information);
    ASSERT_EQ(parsed_message->additional_information.value(), "hey");
}

TEST(SharedLibraryTest, ActionOrderMessageTwoWayConversion)
{
    ActionOrderMessage original_message("123", "456");

    std::string json = original_message.to_json();

    std::unique_ptr<ServerToClientMessage> base_message;
    base_message = ServerToClientMessage::from_json(json);

    std::unique_ptr<ActionOrderMessage> parsed_message(dynamic_cast<ActionOrderMessage *>(base_message.release()));

    ASSERT_NE(parsed_message, nullptr);

    ASSERT_EQ(parsed_message->game_id, "123");
    ASSERT_EQ(parsed_message->message_id, "456");
    ASSERT_FALSE(parsed_message->description);
}

// ======= CLIENT TO SERVER MESSAGES ======= //

TEST(SharedLibraryTest, GameStateRequestMessageTwoWayConversion)
{
    GameStateRequestMessage original_message("123", "456", "player1");

    std::string json = original_message.to_json();

    std::unique_ptr<ClientToServerMessage> base_message;
    base_message = ClientToServerMessage::from_json(json);

    std::unique_ptr<GameStateRequestMessage> parsed_message(
            dynamic_cast<GameStateRequestMessage *>(base_message.release()));

    ASSERT_NE(parsed_message, nullptr);

    ASSERT_EQ(parsed_message->game_id, "123");
    ASSERT_EQ(parsed_message->message_id, "456");
    ASSERT_EQ(parsed_message->player_id, "player1");
}

TEST(SharedLibraryTest, CreateLobbyRequestMessageTwoWayConversion)
{
    CreateLobbyRequestMessage original_message("123", "456", "player1");

    std::string json = original_message.to_json();

    std::unique_ptr<ClientToServerMessage> base_message;
    base_message = ClientToServerMessage::from_json(json);

    std::unique_ptr<CreateLobbyRequestMessage> parsed_message(
            dynamic_cast<CreateLobbyRequestMessage *>(base_message.release()));

    ASSERT_NE(parsed_message, nullptr);

    ASSERT_EQ(parsed_message->game_id, "123");
    ASSERT_EQ(parsed_message->message_id, "456");
    ASSERT_EQ(parsed_message->player_id, "player1");
}

TEST(SharedLibraryTest, JoinLobbyRequestMessageTwoWayConversion)
{
    JoinLobbyRequestMessage original_message("123", "456", "player1");

    std::string json = original_message.to_json();

    std::unique_ptr<ClientToServerMessage> base_message;
    base_message = ClientToServerMessage::from_json(json);

    std::unique_ptr<JoinLobbyRequestMessage> parsed_message(
            dynamic_cast<JoinLobbyRequestMessage *>(base_message.release()));

    ASSERT_NE(parsed_message, nullptr);

    ASSERT_EQ(parsed_message->game_id, "123");
    ASSERT_EQ(parsed_message->message_id, "456");
    ASSERT_EQ(parsed_message->player_id, "player1");
}

TEST(SharedLibraryTest, StartGameRequestMessageTwoWayConversion)
{
    std::vector<std::string> cards = {"village",    "smithy",  "market", "council_room", "festival",
                                      "laboratory", "library", "mine",   "witch",        "adventurer"};
    StartGameRequestMessage original_message("123", "456", "player1", cards);

    std::string json = original_message.to_json();

    std::unique_ptr<ClientToServerMessage> base_message;
    base_message = ClientToServerMessage::from_json(json);

    std::unique_ptr<StartGameRequestMessage> parsed_message(
            dynamic_cast<StartGameRequestMessage *>(base_message.release()));

    ASSERT_NE(parsed_message, nullptr);

    ASSERT_EQ(parsed_message->game_id, "123");
    ASSERT_EQ(parsed_message->message_id, "456");
    ASSERT_EQ(parsed_message->player_id, "player1");
    ASSERT_EQ(parsed_message->selected_cards, cards);
}

TEST(SharedLibraryTest, ActionDecisionMessageTwoWayConversionPlayActionCard)
{
    std::unique_ptr<ActionDecision> decision = std::make_unique<PlayActionCardDecision>(1);
    ActionDecisionMessage original_message("123", "456", "player1", std::move(decision), "789");

    std::string json = original_message.to_json();

    std::unique_ptr<ClientToServerMessage> base_message;
    base_message = ClientToServerMessage::from_json(json);

    std::unique_ptr<ActionDecisionMessage> parsed_message(
            dynamic_cast<ActionDecisionMessage *>(base_message.release()));

    ASSERT_NE(parsed_message, nullptr);

    ASSERT_EQ(parsed_message->game_id, "123");
    ASSERT_EQ(parsed_message->message_id, "456");
    ASSERT_EQ(parsed_message->player_id, "player1");
    ASSERT_TRUE(parsed_message->in_response_to);
    ASSERT_EQ(parsed_message->in_response_to.value(), "789");

    ASSERT_NE(parsed_message->decision, nullptr);
    PlayActionCardDecision *parsed_decision = dynamic_cast<PlayActionCardDecision *>(parsed_message->decision.get());
    ASSERT_NE(parsed_decision, nullptr);
    ASSERT_EQ(parsed_decision->cardIndex, 1);
}

TEST(SharedLibraryTest, ActionDecisionMessageTwoWayConversionBuyCard)
{
    ActionDecisionMessage original_message("123", "456", "player1", std::make_unique<BuyCardDecision>("copper"));

    std::string json = original_message.to_json();

    std::unique_ptr<ClientToServerMessage> base_message;
    base_message = ClientToServerMessage::from_json(json);

    std::unique_ptr<ActionDecisionMessage> parsed_message(
            dynamic_cast<ActionDecisionMessage *>(base_message.release()));

    ASSERT_NE(parsed_message, nullptr);

    ASSERT_EQ(parsed_message->game_id, "123");
    ASSERT_EQ(parsed_message->message_id, "456");
    ASSERT_EQ(parsed_message->player_id, "player1");
    ASSERT_FALSE(parsed_message->in_response_to);
    ASSERT_EQ(parsed_message->player_id, "player1");

    ASSERT_NE(parsed_message->decision, nullptr);
    BuyCardDecision *parsed_decision = dynamic_cast<BuyCardDecision *>(parsed_message->decision.get());
    ASSERT_NE(parsed_decision, nullptr);
    ASSERT_EQ(parsed_decision->card, "copper");
}

TEST(SharedLibraryTest, ActionDecisionMessageTwoWayConversionEndTurn)
{
    ActionDecisionMessage original_message("123", "456", "player1", std::make_unique<EndTurnDecision>(), "789");

    std::string json = original_message.to_json();

    std::unique_ptr<ClientToServerMessage> base_message;
    base_message = ClientToServerMessage::from_json(json);

    std::unique_ptr<ActionDecisionMessage> parsed_message(
            dynamic_cast<ActionDecisionMessage *>(base_message.release()));

    ASSERT_NE(parsed_message, nullptr);

    ASSERT_EQ(parsed_message->game_id, "123");
    ASSERT_EQ(parsed_message->message_id, "456");
    ASSERT_EQ(parsed_message->player_id, "player1");
    ASSERT_TRUE(parsed_message->in_response_to);
    ASSERT_EQ(parsed_message->in_response_to.value(), "789");

    ASSERT_NE(parsed_message->decision, nullptr);
    EndTurnDecision *parsed_decision = dynamic_cast<EndTurnDecision *>(parsed_message->decision.get());
    ASSERT_NE(parsed_decision, nullptr);
}

TEST(SharedLibraryTest, ActionDecisionMessageTwoWayConversionChooseNCardsFromHand)
{
    std::vector<unsigned int> card_indices = {0, 2, 3};
    std::unique_ptr<ActionDecision> decision = std::make_unique<ChooseNCardsFromHandDecision>(card_indices);
    ActionDecisionMessage original_message("123", "456", "player1", std::move(decision));

    std::string json = original_message.to_json();

    std::unique_ptr<ClientToServerMessage> base_message;
    base_message = ClientToServerMessage::from_json(json);

    std::unique_ptr<ActionDecisionMessage> parsed_message(
            dynamic_cast<ActionDecisionMessage *>(base_message.release()));

    ASSERT_NE(parsed_message, nullptr);

    ASSERT_EQ(parsed_message->game_id, "123");
    ASSERT_EQ(parsed_message->message_id, "456");
    ASSERT_EQ(parsed_message->player_id, "player1");
    ASSERT_FALSE(parsed_message->in_response_to);

    ASSERT_NE(parsed_message->decision, nullptr);
    ChooseNCardsFromHandDecision *parsed_decision =
            dynamic_cast<ChooseNCardsFromHandDecision *>(parsed_message->decision.get());
    ASSERT_NE(parsed_decision, nullptr);
    ASSERT_EQ(parsed_decision->cards, card_indices);
}
