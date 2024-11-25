#include <shared/game/game_state/player_base.h>

#include <gtest/gtest.h>

TEST(PlayerJsonConversions, ReducedPlayer2Way)
{
    shared::PlayerBase base_player("Alice");
    std::vector<shared::CardBase::id_t> hand_cards = {"Province", "Copper", "Copper", "Village", "Estate"};
    auto expected = reduced::Player::make(base_player, hand_cards);
    auto json = expected->toJson();
    std::unique_ptr<reduced::Player> actual = reduced::Player::fromJson(json);
    EXPECT_EQ(*actual, *expected);
}

TEST(PlayerJsonConversions, ReducedEnemy2Way)
{
    shared::PlayerBase base_player("Bob");
    unsigned int hand_size = 3;
    auto expected = reduced::Enemy::make(base_player, hand_size);
    auto json = expected->toJson();
    std::unique_ptr<reduced::Enemy> actual = reduced::Enemy::fromJson(json);
    EXPECT_EQ(*actual, *expected);
}

class TestablePlayerBase : public shared::PlayerBase
{
public:
    using shared::PlayerBase::PlayerBase;

    // Expose protected variables through public getters
    const shared::CardBase::id_t getCurrentCard() const { return current_card; }
    const std::vector<shared::CardBase::id_t> getDiscardPile() const { return discard_pile; }
    unsigned int getDrawPileSize() const { return draw_pile_size; }
    const std::vector<shared::CardBase::id_t> getPlayedCards() const { return played_cards; }

    void setVictoryPoints(unsigned int points) { victory_points = points; }
    void setActions(unsigned int action_count) { actions = action_count; }
    void setBuys(unsigned int buy_count) { buys = buy_count; }
    void setTreasure(unsigned int treasure_count) { treasure = treasure_count; }
};

TEST(PlayerBaseTest, Constructor)
{
    const std::string player_id = "player1";
    TestablePlayerBase player(player_id);

    EXPECT_EQ(player.getId(), player_id);

    EXPECT_EQ(player.getVictoryPoints(), 0);
    EXPECT_EQ(player.getActions(), 1);
    EXPECT_EQ(player.getBuys(), 1);
    EXPECT_EQ(player.getTreasure(), 0);

    // testing private variables
    EXPECT_EQ(player.getCurrentCard(), "");
    EXPECT_EQ(player.getDiscardPile().empty(), true);
    EXPECT_EQ(player.getDrawPileSize(), 0);
    EXPECT_EQ(player.getPlayedCards().size(), 0);
}

TEST(PlayerBaseTest, CopyConstructor)
{
    shared::PlayerBase original("player1");
    shared::PlayerBase copy(original);

    EXPECT_EQ(copy, original);
}

TEST(PlayerBaseTest, DecreaseAvailableActions)
{
    shared::PlayerBase player("player");

    EXPECT_EQ(player.getActions(), 1); // initialised correctly?

    player.decActions();
    EXPECT_EQ(player.getActions(), 0); // should dec by one -> 0

    player.decActions();
    EXPECT_EQ(player.getActions(), 0); // cant decrease again, still zero
}

TEST(PlayerBaseTest, DecreaseAvailableBuys)
{
    shared::PlayerBase player("player");

    EXPECT_EQ(player.getBuys(), 1); // initialised correctly?

    player.decBuys();
    EXPECT_EQ(player.getBuys(), 0); // should dec by one -> 0

    player.decBuys();
    EXPECT_EQ(player.getBuys(), 0); // cant decrease again, still zero
}

TEST(PlayerBaseTest, DecreaseAvailableTreasureBasic)
{
    shared::PlayerBase player("player");

    EXPECT_EQ(player.getTreasure(), 0); // initialised correctly?

    player.decTreasure(1);
    EXPECT_EQ(player.getTreasure(), 0); // should still be zero
}

TEST(PlayerBaseTest, DecreaseAvailableTreasureHardcore)
{
    TestablePlayerBase player("player");

    EXPECT_EQ(player.getTreasure(), 0); // initialised correctly?

    player.setTreasure(100);
    EXPECT_EQ(player.getTreasure(), 100);

    player.decTreasure(1);
    EXPECT_EQ(player.getTreasure(), 99);

    player.decTreasure(9);
    EXPECT_EQ(player.getTreasure(), 90);

    player.decTreasure(91);
    EXPECT_EQ(player.getTreasure(), 90); // edge case (▀̿Ĺ̯▀̿ ̿)

    player.decTreasure(-91);
    EXPECT_EQ(player.getTreasure(), 90); // overflow safe ig?
}

TEST(ReducedEnemyTest, Initialization)
{
    // Set up a PlayerBase object
    TestablePlayerBase base_player("test_player");
    base_player.setVictoryPoints(10);
    base_player.setActions(2);
    base_player.setBuys(1);
    base_player.setTreasure(5);

    // Initialize ReducedEnemy with PlayerBase and specific hand_size
    unsigned int hand_size = 3;
    auto reduced_enemy = reduced::Enemy::make(base_player, hand_size);

    // Check that ReducedEnemy inherited PlayerBase attributes correctly
    EXPECT_EQ(reduced_enemy->getId(), "test_player");
    EXPECT_EQ(reduced_enemy->getVictoryPoints(), 10);
    EXPECT_EQ(reduced_enemy->getActions(), 2);
    EXPECT_EQ(reduced_enemy->getBuys(), 1);
    EXPECT_EQ(reduced_enemy->getTreasure(), 5);

    // Check that ReducedEnemy-specific attribute is set
    EXPECT_EQ(reduced_enemy->getHandSize(), hand_size);
}

TEST(ReducedPlayerTest, Initialization)
{
    // Set up a PlayerBase object
    TestablePlayerBase base_player("test_player_2");
    base_player.setVictoryPoints(15);
    base_player.setActions(3);
    base_player.setBuys(2);
    base_player.setTreasure(8);

    // Define a hand of cards for ReducedPlayer
    std::vector<shared::CardBase::id_t> hand_cards = {"card1", "card2", "card3"};

    // Initialize ReducedPlayer with PlayerBase and specific hand_cards
    auto reduced_player = reduced::Player::make(base_player, hand_cards);

    // Check that ReducedPlayer inherited PlayerBase attributes correctly
    EXPECT_EQ(reduced_player->getId(), "test_player_2");
    EXPECT_EQ(reduced_player->getVictoryPoints(), 15);
    EXPECT_EQ(reduced_player->getActions(), 3);
    EXPECT_EQ(reduced_player->getBuys(), 2);
    EXPECT_EQ(reduced_player->getTreasure(), 8);

    // Check that ReducedPlayer-specific attribute is set
    EXPECT_EQ(reduced_player->getHandCards(), hand_cards);
}
