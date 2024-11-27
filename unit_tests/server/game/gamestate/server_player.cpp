#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <server/game/server_player.h>

using namespace server;

class TestPlayer : public Player
{
public:
    using Player::Player;

    // to make setups easier
    using Player::getMutable;

    // expose the functions to test
};

// Begin test cases
TEST(PlayerTest, DefaultConstructor)
{
    TestPlayer player("player");

    EXPECT_EQ(player.getId(), "player");
    EXPECT_EQ(player.getVictoryPoints(), 0);
    EXPECT_EQ(player.getActions(), 1);
    EXPECT_EQ(player.getBuys(), 1);
    EXPECT_EQ(player.getTreasure(), 0);

    EXPECT_TRUE(player.get<shared::CardAccess::DRAW_PILE_TOP>().empty());
    EXPECT_TRUE(player.get<shared::CardAccess::DISCARD_PILE>().empty());
    EXPECT_TRUE(player.get<shared::CardAccess::HAND>().empty());
}

TEST(PlayerTest, ConstructorWithId)
{
    std::string player_id = "player1";
    TestPlayer player(player_id);
    EXPECT_EQ(player.getId(), player_id);
}

TEST(PlayerTest, CopyConstructor)
{
    std::string player_id = "player1";
    TestPlayer player(player_id);

    // initialise some properties
    player.addActions(2);
    player.addBuys(1);
    player.addTreasure(3);
    player.addPoints(5);

    TestPlayer copy_player(player);

    // check that copy has the same properties
    EXPECT_EQ(copy_player.getId(), player.getId());
    EXPECT_EQ(copy_player.getActions(), player.getActions());
    EXPECT_EQ(copy_player.getBuys(), player.getBuys());
    EXPECT_EQ(copy_player.getTreasure(), player.getTreasure());
    EXPECT_EQ(copy_player.getVictoryPoints(), player.getVictoryPoints());
}

TEST(PlayerTest, DrawCards)
{
    TestPlayer player("player");

    std::vector<std::string> draw_pile = {"Card1", "Card2", "Card3", "Card4", "Card5"};
    player.getMutable<shared::CardAccess::DRAW_PILE_TOP>() = draw_pile;

    player.draw(2);

    ASSERT_EQ(player.get<shared::CardAccess::HAND>().size(), 2);
    EXPECT_EQ(player.get<shared::CardAccess::HAND>()[0], "Card1");
    EXPECT_EQ(player.get<shared::CardAccess::HAND>()[1], "Card2");

    ASSERT_EQ(player.get<shared::CardAccess::DRAW_PILE_TOP>().size(), 3);
    EXPECT_EQ(player.get<shared::CardAccess::DRAW_PILE_TOP>()[0], "Card3");
    EXPECT_EQ(player.get<shared::CardAccess::DRAW_PILE_TOP>()[1], "Card4");
    EXPECT_EQ(player.get<shared::CardAccess::DRAW_PILE_TOP>()[2], "Card5");
}

TEST(PlayerTest, TrashCard)
{
    TestPlayer player("player");
    std::vector<std::string> hand = {"Card1", "Card2", "Card3"};
    player.getMutable<shared::CardAccess::HAND>() = hand;

    std::vector<unsigned int> indices = {1};
    player.trash<shared::CardAccess::HAND>(indices);

    ASSERT_EQ(player.get<shared::CardAccess::HAND>().size(), 2);
    EXPECT_EQ(player.get<shared::CardAccess::HAND>()[0], "Card1");
    EXPECT_EQ(player.get<shared::CardAccess::HAND>()[1], "Card3");
}

TEST(PlayerTest, DiscardCard)
{
    TestPlayer player("player");
    std::vector<std::string> hand = {"Card1", "Card2", "Card3"};
    player.getMutable<shared::CardAccess::HAND>() = hand;

    // Discard the second card (index 1)
    std::vector<unsigned int> indices = {1};
    player.discard<shared::CardAccess::HAND>(indices);

    // Now hand should have "Card1", "Card3"
    ASSERT_EQ(player.get<shared::CardAccess::HAND>().size(), 2);
    EXPECT_EQ(player.get<shared::CardAccess::HAND>()[0], "Card1");
    EXPECT_EQ(player.get<shared::CardAccess::HAND>()[1], "Card3");

    // Discard pile should have "Card2"
    ASSERT_EQ(player.get<shared::CardAccess::DISCARD_PILE>().size(), 1);
    EXPECT_EQ(player.get<shared::CardAccess::DISCARD_PILE>()[0], "Card2");
}

TEST(PlayerTest, GainCard)
{
    TestPlayer player("player");
    std::vector<std::string> discard_pile = {"Card1", "Card2"};
    player.getMutable<shared::CardAccess::DISCARD_PILE>() = discard_pile;

    // Add "Card3" to hand
    player.gain("Card3");

    // Now hand should have "Card1", "Card2", "Card3"
    ASSERT_EQ(player.get<shared::CardAccess::DISCARD_PILE>().size(), 3);
    EXPECT_EQ(player.get<shared::CardAccess::DISCARD_PILE>()[2], "Card3");
}

TEST(PlayerTest, AddToDiscardPile)
{
    TestPlayer player("player");

    // Discard pile is initially empty
    EXPECT_TRUE(player.get<shared::CardAccess::DISCARD_PILE>().empty());

    // Add "Card1" to discard pile
    player.gain("Card1");

    // Now discard pile should have "Card1"
    ASSERT_EQ(player.get<shared::CardAccess::DISCARD_PILE>().size(), 1);
    EXPECT_EQ(player.get<shared::CardAccess::DISCARD_PILE>()[0], "Card1");
}

TEST(PlayerTest, IncreaseActions)
{
    TestPlayer player("player");
    EXPECT_EQ(player.getActions(), 1);

    player.addActions(2);
    EXPECT_EQ(player.getActions(), 3);
}

TEST(PlayerTest, IncreaseBuys)
{
    TestPlayer player("player");
    EXPECT_EQ(player.getBuys(), 1);

    player.addBuys(3);
    EXPECT_EQ(player.getBuys(), 4);
}

TEST(PlayerTest, IncreaseTreasure)
{
    TestPlayer player("player");
    EXPECT_EQ(player.getTreasure(), 0);

    player.addTreasure(5);
    EXPECT_EQ(player.getTreasure(), 5);
}

TEST(PlayerTest, AddPoints)
{
    TestPlayer player("player");
    EXPECT_EQ(player.getVictoryPoints(), 0);

    player.addPoints(7);
    EXPECT_EQ(player.getVictoryPoints(), 7);
}

TEST(PlayerTest, EndTurn)
{
    TestPlayer player("player");
    // Set up some state
    player.addActions(2);
    player.addBuys(1);
    player.addTreasure(3);
    player.addPoints(5);
    player.getMutable<shared::CardAccess::HAND>() = {"Card1", "Card2"};
    player.getMutable<shared::CardAccess::PLAYED_CARDS>() = {"Card3"};
    player.getMutable<shared::CardAccess::DISCARD_PILE>() = {"Card4"};

    // Call end_turn()
    player.endTurn();

    // Check that values are reset
    EXPECT_EQ(player.getActions(), 1);
    EXPECT_EQ(player.getBuys(), 1);
    EXPECT_EQ(player.getTreasure(), 0);

    // hand should contain 5 cards again
    EXPECT_EQ(player.get<shared::CardAccess::HAND>().size(), 4);

    // discard pile should be empty as reshuffle was triggered
    ASSERT_EQ(player.get<shared::CardAccess::DISCARD_PILE>().size(), 0); // hand + played + pile
}

TEST(PlayerTest, AddCardsToPile)
{
    TestPlayer player("player");

    // Set up an empty discard pile
    player.getMutable<shared::CardAccess::DISCARD_PILE>().clear();

    // Add cards to the discard pile
    player.gain("Card1");
    player.gain("Card2");
    player.gain("Card3");

    // Verify the discard pile
    const auto &discard_pile = player.get<shared::CardAccess::DISCARD_PILE>();

    ASSERT_EQ(discard_pile.size(), 3);
    EXPECT_EQ(discard_pile[0], "Card1");
    EXPECT_EQ(discard_pile[1], "Card2");
    EXPECT_EQ(discard_pile[2], "Card3");
}

TEST(PlayerTest, GetPile)
{
    TestPlayer player("player");

    // Set up piles
    player.getMutable<shared::CardAccess::DISCARD_PILE>() = {"Card1", "Card2"};
    player.getMutable<shared::CardAccess::DRAW_PILE_TOP>() = {"Card3", "Card4"};
    player.getMutable<shared::CardAccess::HAND>() = {"Card5"};
    player.getMutable<shared::CardAccess::PLAYED_CARDS>() = {"Card6"};

    // Access and verify each pile
    EXPECT_EQ(player.get<shared::CardAccess::DISCARD_PILE>()[0], "Card1");
    EXPECT_EQ(player.get<shared::CardAccess::DISCARD_PILE>()[1], "Card2");

    EXPECT_EQ(player.get<shared::CardAccess::DRAW_PILE_TOP>()[0], "Card3");
    EXPECT_EQ(player.get<shared::CardAccess::DRAW_PILE_TOP>()[1], "Card4");

    EXPECT_EQ(player.get<shared::CardAccess::HAND>()[0], "Card5");

    EXPECT_EQ(player.get<shared::CardAccess::PLAYED_CARDS>()[0], "Card6");
}
