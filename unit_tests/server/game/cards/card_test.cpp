// card_server_test.cpp

#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <tuple>

// Include the header files for the classes we are testing
#include <server/game/cards/cards.h>

// Mock definitions and minimal implementations for dependencies
namespace server
{

    // Mock implementation of Player
    class Player : public shared::PlayerBase
    {
    public:
        using id_t = shared::PlayerBase::id_t;

        Player(id_t id) : shared::PlayerBase(id) {}

        void increase_treasure(int n) { available_treasure += n; }
        void increase_buys(int n) { available_buys += n; }
        void increase_actions(int n) { available_actions += n; }
        void add_points(int n) { victory_points += n; }

        unsigned int getTreasure() const { return available_treasure; }
        unsigned int getBuys() const { return available_buys; }
        unsigned int getActions() const { return available_actions; }
        unsigned int getVictoryPoints() const { return victory_points; }
    };

    // Mock implementation of GameState
    class GameState
    {
    public:
        // Add player to the game state
        void add_player(const Player::id_t &id) { players.emplace(id, Player(id)); }

        // Get player by ID
        Player &get_player(const Player::id_t &id) { return players.at(id); }

    private:
        std::unordered_map<Player::id_t, Player> players;
    };

    // Since the behaviours are templates, we need to instantiate them
    namespace behaviour
    {
        // Implementations are provided in the code snippet you gave
        // We can use the existing templates directly
    }

} // namespace server

// Begin test cases

// Fixture class for testing cards and behaviors
class CardServerTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Initialize game state and players
        game_state.add_player("player1");
        affected_player_id = "player1";
    }

    // Helper function to create a card with specified behaviors
    template <typename CardType>
    std::unique_ptr<CardType> createCard(const std::string &id, shared::CardType type, unsigned int cost)
    {
        return std::make_unique<CardType>(id, type, cost);
    }

    server::GameState game_state;
    server::Player::id_t affected_player_id;
};

TEST_F(CardServerTest, GainCoinsBehaviourTest)
{
    // Create a card with GainCoins<3> behavior
    using GainCoinsCard = server::Card<server::behaviour::GainCoins<3>>;

    auto card = createCard<GainCoinsCard>("GainCoinsCard", shared::TREASURE, 5);

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

TEST_F(CardServerTest, GainBuysBehaviourTest)
{
    // Create a card with GainBuys<2> behavior
    using GainBuysCard = server::Card<server::behaviour::GainBuys<2>>;

    auto card = createCard<GainBuysCard>("GainBuysCard", shared::ACTION, 4);

    // Before playing the card
    auto &player = game_state.get_player(affected_player_id);
    EXPECT_EQ(player.getBuys(), 0);

    // Play the card
    int next_behaviour_idx = card->play(game_state, affected_player_id, 0);

    // After playing the card
    EXPECT_EQ(player.getBuys(), 2);

    // Ensure that next_behaviour_idx indicates completion
    EXPECT_EQ(next_behaviour_idx, 1);
}

TEST_F(CardServerTest, GainActionsBehaviourTest)
{
    // Create a card with GainActions<1> behavior
    using GainActionsCard = server::Card<server::behaviour::GainActions<1>>;

    auto card = createCard<GainActionsCard>("GainActionsCard", shared::ACTION, 3);

    // Before playing the card
    auto &player = game_state.get_player(affected_player_id);
    EXPECT_EQ(player.getActions(), 0);

    // Play the card
    int next_behaviour_idx = card->play(game_state, affected_player_id, 0);

    // After playing the card
    EXPECT_EQ(player.getActions(), 1);

    // Ensure that next_behaviour_idx indicates completion
    EXPECT_EQ(next_behaviour_idx, 1);
}

TEST_F(CardServerTest, GainPointsBehaviourTest)
{
    // Create a card with GainPoints<2> behavior
    using GainPointsCard = server::Card<server::behaviour::GainPoints<2>>;

    auto card = createCard<GainPointsCard>("GainPointsCard", shared::VICTORY, 2);

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

TEST_F(CardServerTest, MultipleBehavioursTest)
{
    // Create a card with multiple behaviors: GainActions<1>, GainBuys<1>, GainCoins<1>
    using MultiBehaviourCard = server::Card<server::behaviour::GainActions<1>, server::behaviour::GainBuys<1>,
                                            server::behaviour::GainCoins<1>>;

    auto card = createCard<MultiBehaviourCard>("MultiBehaviourCard", shared::ACTION, 4);

    auto &player = game_state.get_player(affected_player_id);

    // Before playing the card
    EXPECT_EQ(player.getActions(), 0);
    EXPECT_EQ(player.getBuys(), 0);
    EXPECT_EQ(player.getTreasure(), 0);

    // Play the card, step through each behaviour
    int behaviour_idx = 0;
    while ( behaviour_idx < card->size() ) {
        behaviour_idx = card->play(game_state, affected_player_id, behaviour_idx);
    }

    // After playing the card
    EXPECT_EQ(player.getActions(), 1);
    EXPECT_EQ(player.getBuys(), 1);
    EXPECT_EQ(player.getTreasure(), 1);

    // Ensure that behaviour_idx indicates completion
    EXPECT_EQ(behaviour_idx, 3); // There are three behaviours
}

TEST_F(CardServerTest, InvalidBehaviourIndexTest)
{
    // Create a card with GainCoins<2> behavior
    using GainCoinsCard = server::Card<server::behaviour::GainCoins<2>>;

    auto card = createCard<GainCoinsCard>("GainCoinsCard", shared::TREASURE, 5);

    auto &player = game_state.get_player(affected_player_id);

    // Play the card with an invalid behaviour index
    int next_behaviour_idx = card->play(game_state, affected_player_id, 5); // Invalid index

    // The play method should return size() indicating completion
    EXPECT_EQ(next_behaviour_idx, card->size());

    // Player's treasure should remain unchanged
    EXPECT_EQ(player.getTreasure(), 0);
}

TEST_F(CardServerTest, BehaviourNotDoneYetTest)
{
    // For this test, we need a behaviour that returns NOT_DONE_YET
    // We'll create a mock behaviour for this purpose

    struct MockBehaviourNotDoneYet : public server::behaviour::BehaviourBase
    {
        bool apply(server::GameState &, server::Player::id_t &) override
        {
            // Simulate a behaviour that is not done yet
            return server::behaviour::NOT_DONE_YET;
        }
    };

    // Create a card with the mock behaviour
    using MockCard = server::Card<MockBehaviourNotDoneYet>;

    auto card = createCard<MockCard>("MockCard", shared::ACTION, 1);

    // Play the card
    int next_behaviour_idx = card->play(game_state, affected_player_id, 0);

    // Since the behaviour is not done yet, next_behaviour_idx should remain the same
    EXPECT_EQ(next_behaviour_idx, 0);
}

TEST_F(CardServerTest, ApplyBehaviourAtIndexTest)
{
    // Create a card with multiple behaviors
    using MultiBehaviourCard = server::Card<server::behaviour::GainActions<1>, server::behaviour::GainBuys<1>,
                                            server::behaviour::GainCoins<1>>;

    auto card = createCard<MultiBehaviourCard>("MultiBehaviourCard", shared::ACTION, 4);

    auto &player = game_state.get_player(affected_player_id);

    // Apply only the second behaviour (GainBuys<1>)
    int next_behaviour_idx = card->play(game_state, affected_player_id, 1);

    // After playing the second behaviour
    EXPECT_EQ(player.getBuys(), 1);
    EXPECT_EQ(next_behaviour_idx, 2);

    // Actions and Treasure should remain unchanged
    EXPECT_EQ(player.getActions(), 0);
    EXPECT_EQ(player.getTreasure(), 0);
}
