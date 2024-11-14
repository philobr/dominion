#include <gmock/gmock.h>
#include "gtest/gtest.h"

#include <server/game/game_state/server_player.h>

// Begin test cases
TEST(PlayerTest, DefaultConstructor)
{
    server::Player player("player");

    EXPECT_EQ(player.getId(), "player");
    EXPECT_EQ(player.getVictoryPoints(), 0);
    EXPECT_EQ(player.getActions(), 1);
    EXPECT_EQ(player.getBuys(), 1);
    EXPECT_EQ(player.getTreasure(), 0);

    EXPECT_TRUE(player.get_draw_pile().empty());
    EXPECT_TRUE(player.get_discard_pile().empty());
    EXPECT_TRUE(player.get_hand_cards().empty());
}

TEST(PlayerTest, ConstructorWithId)
{
    std::string player_id = "player1";
    server::Player player(player_id);
    EXPECT_EQ(player.getId(), player_id);
}

TEST(PlayerTest, CopyConstructor)
{
    std::string player_id = "player1";
    server::Player player(player_id);

    // initialise some properties
    player.incActions(2);
    player.incBuys(1);
    player.incTreasure(3);
    player.incPoints(5);

    server::Player copy_player(player);

    // check that copy has the same properties
    EXPECT_EQ(copy_player.getId(), player.getId());
    EXPECT_EQ(copy_player.getActions(), player.getActions());
    EXPECT_EQ(copy_player.getBuys(), player.getBuys());
    EXPECT_EQ(copy_player.getTreasure(), player.getTreasure());
    EXPECT_EQ(copy_player.getVictoryPoints(), player.getVictoryPoints());
}

TEST(PlayerTest, PeekDrawPile)
{
    server::Player player("player");
    // Set up draw_pile
    std::deque<std::string> draw_pile = {"Card1", "Card2", "Card3", "Card4", "Card5"};
    player.get_draw_pile() = draw_pile;

    // Peek top 3 cards
    std::vector<std::string> peeked_cards = player.peek_draw_pile(3);

    ASSERT_EQ(peeked_cards.size(), 3);
    EXPECT_EQ(peeked_cards[0], "Card1");
    EXPECT_EQ(peeked_cards[1], "Card2");
    EXPECT_EQ(peeked_cards[2], "Card3");

    // Ensure draw_pile is unchanged
    EXPECT_EQ(player.get_draw_pile().size(), 5);
}

TEST(PlayerTest, DrawCards)
{
    server::Player player("player");

    std::deque<std::string> draw_pile = {"Card1", "Card2", "Card3", "Card4", "Card5"};
    player.get_draw_pile() = draw_pile;

    player.draw(2);

    ASSERT_EQ(player.get_hand_cards().size(), 2);
    EXPECT_EQ(player.get_hand_cards()[0], "Card1");
    EXPECT_EQ(player.get_hand_cards()[1], "Card2");

    ASSERT_EQ(player.get_draw_pile().size(), 3);
    EXPECT_EQ(player.get_draw_pile()[0], "Card3");
    EXPECT_EQ(player.get_draw_pile()[1], "Card4");
    EXPECT_EQ(player.get_draw_pile()[2], "Card5");
}

TEST(PlayerTest, TrashCard)
{
    server::Player player("player");
    std::vector<std::string> hand = {"Card1", "Card2", "Card3"};
    player.get_hand_cards() = hand;

    player.trash(player.get_hand_cards(), 1);

    ASSERT_EQ(player.get_hand_cards().size(), 2);
    EXPECT_EQ(player.get_hand_cards()[0], "Card1");
    EXPECT_EQ(player.get_hand_cards()[1], "Card3");
}

TEST(PlayerTest, DiscardCard)
{
    server::Player player("player");
    std::vector<std::string> hand = {"Card1", "Card2", "Card3"};
    player.get_hand_cards() = hand;

    // Discard the second card (index 1)
    player.discard(player.get_hand_cards(), 1);

    // Now hand should have "Card1", "Card3"
    ASSERT_EQ(player.get_hand_cards().size(), 2);
    EXPECT_EQ(player.get_hand_cards()[0], "Card1");
    EXPECT_EQ(player.get_hand_cards()[1], "Card3");

    // Discard pile should have "Card2"
    ASSERT_EQ(player.get_discard_pile().size(), 1);
    EXPECT_EQ(player.get_discard_pile()[0], "Card2");
}

TEST(PlayerTest, AddCard)
{
    server::Player player("player");
    std::vector<std::string> hand = {"Card1", "Card2"};
    player.get_hand_cards() = hand;

    // Add "Card3" to hand
    player.add(player.get_hand_cards(), "Card3");

    // Now hand should have "Card1", "Card2", "Card3"
    ASSERT_EQ(player.get_hand_cards().size(), 3);
    EXPECT_EQ(player.get_hand_cards()[2], "Card3");
}

TEST(PlayerTest, AddToDiscardPile)
{
    server::Player player("player");

    // Discard pile is initially empty
    EXPECT_TRUE(player.get_discard_pile().empty());

    // Add "Card1" to discard pile
    player.add_to_discard_pile("Card1");

    // Now discard pile should have "Card1"
    ASSERT_EQ(player.get_discard_pile().size(), 1);
    EXPECT_EQ(player.get_discard_pile()[0], "Card1");
}

TEST(PlayerTest, IncreaseActions)
{
    server::Player player("player");
    EXPECT_EQ(player.getActions(), 1);

    player.incActions(2);
    EXPECT_EQ(player.getActions(), 3);
}

TEST(PlayerTest, IncreaseBuys)
{
    server::Player player("player");
    EXPECT_EQ(player.getBuys(), 1);

    player.incBuys(3);
    EXPECT_EQ(player.getBuys(), 4);
}

TEST(PlayerTest, IncreaseTreasure)
{
    server::Player player("player");
    EXPECT_EQ(player.getTreasure(), 0);

    player.incTreasure(5);
    EXPECT_EQ(player.getTreasure(), 5);
}

TEST(PlayerTest, AddPoints)
{
    server::Player player("player");
    EXPECT_EQ(player.getVictoryPoints(), 0);

    player.incPoints(7);
    EXPECT_EQ(player.getVictoryPoints(), 7);
}

TEST(PlayerTest, EndTurn)
{
    server::Player player("player");
    // Set up some state
    player.incActions(2);
    player.incBuys(1);
    player.incTreasure(3);
    player.incPoints(5);
    player.get_hand_cards() = {"Card1", "Card2"};
    player.get_played_cards() = {"Card3"};
    player.get_discard_pile() = {"Card4"};

    // Call end_turn()
    player.end_turn();

    // Check that values are reset
    EXPECT_EQ(player.getActions(), 1);
    EXPECT_EQ(player.getBuys(), 1);
    EXPECT_EQ(player.getTreasure(), 0);

    // hand should contain 5 cards again
    EXPECT_EQ(player.get_hand_cards().size(), 4);

    // discard pile should be empty as reshuffle was triggered
    ASSERT_EQ(player.get_discard_pile().size(), 0); // hand + played + pile
}
