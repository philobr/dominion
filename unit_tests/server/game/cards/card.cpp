#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <tuple>

#include <server/game/cards/cards.h>
#include <server/game/game_state/game_state.h> // Include GameState
#include <server/game/game_state/server_player.h> // Include Player

// Helper function to create a card with specified behaviors
template <typename CardType>
std::unique_ptr<CardType> CreateCard(const std::string &id, shared::CardType type, unsigned int cost)
{
    return std::make_unique<CardType>(id, type, cost);
}

// Begin test cases

TEST(CardServerTest, GainCoinsBehaviourTest)
{
    // Initialize game state and players

    std::vector<shared::CardBase::id_t> playing_cards = {"GainCoinsCard", "card2", "card3", "card4", "card5",
                                                         "card6",         "card7", "card8", "card9", "card10"};

    server::Player::id_t affected_player_id = "player1";
    std::vector<shared::PlayerBase::id_t> players = {affected_player_id, "player2"};
    server::GameState game_state(playing_cards, players);

    // Create a card with GainCoins<3> behavior
    using GainCoinsCard = server::Card<server::behaviour::GainCoins<3>>;

    auto card = CreateCard<GainCoinsCard>("GainCoinsCard", shared::TREASURE, 5);

    // Before playing the card
    auto &player = game_state.get_player(affected_player_id);
    EXPECT_EQ(player.getTreasure(), 0);

    // Play the card
    int next_behaviour_idx = card->play(game_state, affected_player_id, 0);

    // After playing the card
    EXPECT_EQ(player.getTreasure(), 3);

    // Ensure that next_behaviour_idx indicates completion
    EXPECT_EQ(next_behaviour_idx, 1); // Since there is only one behaviour
}

TEST(CardServerTest, GainBuysBehaviourTest)
{
    // Initialize game state and players
    std::vector<shared::CardBase::id_t> playing_cards = {"GainBuysCard", "card2", "card3", "card4", "card5",
                                                         "card6",        "card7", "card8", "card9", "card10"};

    server::Player::id_t affected_player_id = "player1";
    std::vector<shared::PlayerBase::id_t> players = {affected_player_id, "player2"};
    server::GameState game_state(playing_cards, players);

    // Create a card with GainBuys<2> behavior
    using GainBuysCard = server::Card<server::behaviour::GainBuys<2>>;

    auto card = CreateCard<GainBuysCard>("GainBuysCard", shared::ACTION, 4);

    // Before playing the card
    auto &player = game_state.get_player(affected_player_id);
    EXPECT_EQ(player.getBuys(), 1);

    // Play the card
    int next_behaviour_idx = card->play(game_state, affected_player_id, 0);

    // After playing the card
    EXPECT_EQ(player.getBuys(), 3);

    // Ensure that next_behaviour_idx indicates completion
    EXPECT_EQ(next_behaviour_idx, 1);
}

TEST(CardServerTest, GainActionsBehaviourTest)
{
    // Initialize game state and players
    std::vector<shared::CardBase::id_t> playing_cards = {"GainBuysCard", "card2", "card3", "card4", "card5",
                                                         "card6",        "card7", "card8", "card9", "card10"};

    server::Player::id_t affected_player_id = "player1";
    std::vector<shared::PlayerBase::id_t> players = {affected_player_id, "player2"};
    server::GameState game_state(playing_cards, players);

    // Create a card with GainActions<1> behavior
    using GainActionsCard = server::Card<server::behaviour::GainActions<1>>;

    auto card = CreateCard<GainActionsCard>("GainActionsCard", shared::ACTION, 3);

    // Before playing the card
    auto &player = game_state.get_player(affected_player_id);
    EXPECT_EQ(player.getActions(), 1);

    // Play the card
    int next_behaviour_idx = card->play(game_state, affected_player_id, 0);

    // After playing the card
    EXPECT_EQ(player.getActions(), 2);

    // Ensure that next_behaviour_idx indicates completion
    EXPECT_EQ(next_behaviour_idx, 1);
}

TEST(CardServerTest, GainPointsBehaviourTest)
{
    // Initialize game state and players
    std::vector<shared::CardBase::id_t> playing_cards = {"GainPointsCard", "card2", "card3", "card4", "card5",
                                                         "card6",          "card7", "card8", "card9", "card10"};

    server::Player::id_t affected_player_id = "player1";

    std::vector<shared::PlayerBase::id_t> players = {affected_player_id, "player2"};
    server::GameState game_state(playing_cards, players);

    // Create a card with GainPoints<2> behavior
    using GainPointsCard = server::Card<server::behaviour::GainPoints<2>>;

    auto card = CreateCard<GainPointsCard>("GainPointsCard", shared::VICTORY, 2);

    // Before playing the card
    auto &player = game_state.get_player(affected_player_id);
    EXPECT_EQ(player.getVictoryPoints(), 0);

    // Play the card
    int next_behaviour_idx = card->play(game_state, affected_player_id, 0);

    // After playing the card
    EXPECT_EQ(player.getVictoryPoints(), 2);

    // Ensure that next_behaviour_idx indicates completion
    EXPECT_EQ(next_behaviour_idx, 1);
}

TEST(CardServerTest, MultipleBehavioursTest)
{
    // Initialize game state and players
    server::Player::id_t affected_player_id = "player1";
    std::vector<shared::CardBase::id_t> playing_cards = {
            "MultiBehaviourCard", "card2", "card3", "card4", "card5", "card6", "card7", "card8", "card9", "card10"};
    std::vector<shared::PlayerBase::id_t> players = {affected_player_id, "player2"};
    server::GameState game_state(playing_cards, players);

    // Create a card with multiple behaviors: GainActions<1>, GainBuys<1>, GainCoins<1>
    using MultiBehaviourCard = server::Card<server::behaviour::GainActions<1>, server::behaviour::GainBuys<1>,
                                            server::behaviour::GainCoins<1>>;

    auto card = CreateCard<MultiBehaviourCard>("MultiBehaviourCard", shared::ACTION, 4);

    auto &player = game_state.get_player(affected_player_id);

    // Before playing the card
    EXPECT_EQ(player.getActions(), 1);
    EXPECT_EQ(player.getBuys(), 1);
    EXPECT_EQ(player.getTreasure(), 0);

    // Play the card, step through each behaviour
    unsigned int behaviour_idx = 0;
    while ( behaviour_idx < card->size() ) {
        behaviour_idx = card->play(game_state, affected_player_id, behaviour_idx);
    }

    // After playing the card
    EXPECT_EQ(player.getActions(), 2);
    EXPECT_EQ(player.getBuys(), 2);
    EXPECT_EQ(player.getTreasure(), 1);

    // Ensure that behaviour_idx indicates completion
    EXPECT_EQ(behaviour_idx, 3); // There are three behaviours
}

TEST(CardServerTest, InvalidBehaviourIndexTest)
{
    // Initialize game state and players
    std::vector<shared::CardBase::id_t> playing_cards = {"GainCoinsCard", "card2", "card3", "card4", "card5",
                                                         "card6",         "card7", "card8", "card9", "card10"};

    server::Player::id_t affected_player_id = "player1";
    std::vector<shared::PlayerBase::id_t> players = {affected_player_id, "player2"};
    server::GameState game_state(playing_cards, players);

    // Create a card with GainCoins<2> behavior
    using GainCoinsCard = server::Card<server::behaviour::GainCoins<2>>;

    auto card = CreateCard<GainCoinsCard>("GainCoinsCard", shared::TREASURE, 5);

    auto &player = game_state.get_player(affected_player_id);

    // Play the card with an invalid behaviour index
    int next_behaviour_idx = card->play(game_state, affected_player_id, 5); // Invalid index

    // The play method should return size() indicating completion
    EXPECT_EQ(next_behaviour_idx, card->size());

    // Player's treasure should remain unchanged
    EXPECT_EQ(player.getTreasure(), 0);
}

TEST(CardServerTest, BehaviourNotDoneYetTest)
{
    // Initialize game state and players
    std::vector<shared::CardBase::id_t> playing_cards = {"MockCard", "card2", "card3", "card4", "card5",
                                                         "card6",    "card7", "card8", "card9", "card10"};
    server::Player::id_t affected_player_id = "player1";
    std::vector<shared::PlayerBase::id_t> players = {affected_player_id, "player2"};
    server::GameState game_state(playing_cards, players);

    // For this test, we need a behaviour that returns NOT_DONE_YET
    // We'll create a mock behaviour for this purpose

    struct MockBehaviourNotDoneYet : public server::behaviour::BehaviourBase
    {
        bool apply(server::GameState & /*gamestate*/, server::Player::id_t & /*affected_player*/) override
        {
            // Simulate a behaviour that is not done yet
            return server::behaviour::NOT_DONE_YET;
        }
    };

    // Create a card with the mock behaviour
    using MockCard = server::Card<MockBehaviourNotDoneYet>;

    auto card = CreateCard<MockCard>("MockCard", shared::ACTION, 1);

    // Play the card
    int next_behaviour_idx = card->play(game_state, affected_player_id, 0);

    // Since the behaviour is not done yet, next_behaviour_idx should remain the same
    EXPECT_EQ(next_behaviour_idx, 0);
}

TEST(CardServerTest, ApplyBehaviourAtIndexTest)
{
    // Initialize game state and players
    std::vector<shared::CardBase::id_t> playing_cards = {
            "MultiBehaviourCard", "card2", "card3", "card4", "card5", "card6", "card7", "card8", "card9", "card10"};

    server::Player::id_t affected_player_id = "player1";
    std::vector<shared::PlayerBase::id_t> players = {affected_player_id, "player2"};
    server::GameState game_state(playing_cards, players);


    // Create a card with multiple behaviors
    using MultiBehaviourCard = server::Card<server::behaviour::GainActions<1>, server::behaviour::GainBuys<1>,
                                            server::behaviour::GainCoins<1>>;

    auto card = CreateCard<MultiBehaviourCard>("MultiBehaviourCard", shared::ACTION, 4);

    auto &player = game_state.get_player(affected_player_id);

    // Apply only the second behaviour (GainBuys<1>)
    int next_behaviour_idx = card->play(game_state, affected_player_id, 1);

    // After playing the second behaviour
    EXPECT_EQ(player.getBuys(), 2);
    EXPECT_EQ(next_behaviour_idx, 2);

    // Actions and Treasure should remain unchanged
    EXPECT_EQ(player.getActions(), 1);
    EXPECT_EQ(player.getTreasure(), 0);
}
