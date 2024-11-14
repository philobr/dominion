#include <shared/game/game_state/player_base.h>

#include <gtest/gtest.h>

// Derived class specifically for testing
class TestablePlayerBase : public shared::PlayerBase
{
public:
    using PlayerBase::PlayerBase; // Inherit constructors

    // Expose protected variables through public getters
    const shared::CardBase::id_t getCurrentCard() const { return current_card; }
    const std::pair<shared::CardBase::id_t, unsigned int> getDiscardPile() const { return discard_pile; }
    unsigned int getDrawPileSize() const { return draw_pile_size; }
    const std::vector<shared::CardBase::id_t> getPlayedCards() const { return played_cards; }

    void setTreasure(int n) { treasure = n; }
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
    EXPECT_EQ(player.getDiscardPile().first, "");
    EXPECT_EQ(player.getDiscardPile().second, 0);
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
