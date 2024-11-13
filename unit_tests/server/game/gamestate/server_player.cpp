#include <gmock/gmock.h>
#include "gtest/gtest.h"

#include <server/game/game_state/server_player.h>

// Begin test cases
TEST(PlayerTest, DefaultConstructor)
{
    server::Player player;

    EXPECT_EQ(player.getId(), "");
    EXPECT_EQ(player.getVictoryPoints(), 0);
    EXPECT_EQ(player.getAvailableActions(), 1);
    EXPECT_EQ(player.getAvailableBuys(), 1);
    EXPECT_EQ(player.getAvailableTreasure(), 0);

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
    player.increase_actions(2);
    player.increase_buys(1);
    player.increase_treasure(3);
    player.add_points(5);

    server::Player copy_player(player);

    // check that copy has the same properties
    EXPECT_EQ(copy_player.getId(), player.getId());
    EXPECT_EQ(copy_player.getAvailableActions(), player.getAvailableActions());
    EXPECT_EQ(copy_player.getAvailableBuys(), player.getAvailableBuys());
    EXPECT_EQ(copy_player.getAvailableTreasure(), player.getAvailableTreasure());
    EXPECT_EQ(copy_player.getVictoryPoints(), player.getVictoryPoints());
}

TEST(PlayerTest, PeekDrawPile)
{
    server::Player player;
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
    server::Player player;

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
    server::Player player;
    std::vector<std::string> hand = {"Card1", "Card2", "Card3"};
    player.get_hand_cards() = hand;

    player.trash(player.get_hand_cards(), 1);

    ASSERT_EQ(player.get_hand_cards().size(), 2);
    EXPECT_EQ(player.get_hand_cards()[0], "Card1");
    EXPECT_EQ(player.get_hand_cards()[1], "Card3");
}

TEST(PlayerTest, DiscardCard)
{
    server::Player player;
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
    server::Player player;
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
    server::Player player;

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
    server::Player player;
    EXPECT_EQ(player.getAvailableActions(), 1);

    player.increase_actions(2);
    EXPECT_EQ(player.getAvailableActions(), 3);
}

TEST(PlayerTest, IncreaseBuys)
{
    server::Player player;
    EXPECT_EQ(player.getAvailableBuys(), 1);

    player.increase_buys(3);
    EXPECT_EQ(player.getAvailableBuys(), 4);
}

TEST(PlayerTest, IncreaseTreasure)
{
    server::Player player;
    EXPECT_EQ(player.getAvailableTreasure(), 0);

    player.increase_treasure(5);
    EXPECT_EQ(player.getAvailableTreasure(), 5);
}

TEST(PlayerTest, AddPoints)
{
    server::Player player;
    EXPECT_EQ(player.getVictoryPoints(), 0);

    player.add_points(7);
    EXPECT_EQ(player.getVictoryPoints(), 7);
}

TEST(PlayerTest, EndTurn)
{
    server::Player player;
    // Set up some state
    player.increase_actions(2);
    player.increase_buys(1);
    player.increase_treasure(3);
    player.add_points(5);
    player.get_hand_cards() = {"Card1", "Card2"};
    player.get_played_cards() = {"Card3"};
    player.get_discard_pile() = {"Card4"};

    // Call end_turn()
    player.end_turn();

    // Check that values are reset
    EXPECT_EQ(player.getAvailableActions(), 1);
    EXPECT_EQ(player.getAvailableBuys(), 1);
    EXPECT_EQ(player.getAvailableTreasure(), 0);

    // Hand should be empty
    EXPECT_TRUE(player.get_hand_cards().empty());

    // Played cards should be moved to discard pile
    ASSERT_EQ(player.get_discard_pile().size(), 4); // hand + played + pile
}
