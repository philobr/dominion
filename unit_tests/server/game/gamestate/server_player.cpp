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

    EXPECT_TRUE(player.get<server::CardAccess::DRAW_PILE_TOP>().empty());
    EXPECT_TRUE(player.get<server::CardAccess::DISCARD_PILE>().empty());
    EXPECT_TRUE(player.get<server::CardAccess::HAND>().empty());
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
    std::vector<std::string> draw_pile = {"Card1", "Card2", "Card3", "Card4", "Card5"};
    player.get<server::CardAccess::DRAW_PILE_TOP>() = draw_pile;

    // Peek top 3 cards
    player.peek(3);
    std::vector<std::string> peeked_cards = player.get<server::CardAccess::STAGED_CARDS>();
    player.unpeek();

    ASSERT_EQ(peeked_cards.size(), 3);
    EXPECT_EQ(peeked_cards[0], "Card1");
    EXPECT_EQ(peeked_cards[1], "Card2");
    EXPECT_EQ(peeked_cards[2], "Card3");

    // Ensure draw_pile is unchanged
    EXPECT_EQ(player.get<server::CardAccess::DRAW_PILE_TOP>().size(), 5);

    for ( size_t i = 0; i < draw_pile.size(); ++i ) {
        EXPECT_EQ(draw_pile[i], player.get<server::CardAccess::DRAW_PILE_TOP>()[i]);
    }
}

TEST(PlayerTest, DrawCards)
{
    server::Player player("player");

    std::vector<std::string> draw_pile = {"Card1", "Card2", "Card3", "Card4", "Card5"};
    player.get<server::CardAccess::DRAW_PILE_TOP>() = draw_pile;

    player.draw(2);

    ASSERT_EQ(player.get<server::CardAccess::HAND>().size(), 2);
    EXPECT_EQ(player.get<server::CardAccess::HAND>()[0], "Card1");
    EXPECT_EQ(player.get<server::CardAccess::HAND>()[1], "Card2");

    ASSERT_EQ(player.get<server::CardAccess::DRAW_PILE_TOP>().size(), 3);
    EXPECT_EQ(player.get<server::CardAccess::DRAW_PILE_TOP>()[0], "Card3");
    EXPECT_EQ(player.get<server::CardAccess::DRAW_PILE_TOP>()[1], "Card4");
    EXPECT_EQ(player.get<server::CardAccess::DRAW_PILE_TOP>()[2], "Card5");
}

TEST(PlayerTest, TrashCard)
{
    server::Player player("player");
    std::vector<std::string> hand = {"Card1", "Card2", "Card3"};
    player.get<server::CardAccess::HAND>() = hand;

    std::vector<unsigned int> indices = {1};
    player.trash<server::CardAccess::HAND>(indices);

    ASSERT_EQ(player.get<server::CardAccess::HAND>().size(), 2);
    EXPECT_EQ(player.get<server::CardAccess::HAND>()[0], "Card1");
    EXPECT_EQ(player.get<server::CardAccess::HAND>()[1], "Card3");
}

TEST(PlayerTest, DiscardCard)
{
    server::Player player("player");
    std::vector<std::string> hand = {"Card1", "Card2", "Card3"};
    player.get<server::CardAccess::HAND>() = hand;

    // Discard the second card (index 1)
    std::vector<unsigned int> indices = {1};
    player.discard<server::CardAccess::HAND>(indices);

    // Now hand should have "Card1", "Card3"
    ASSERT_EQ(player.get<server::CardAccess::HAND>().size(), 2);
    EXPECT_EQ(player.get<server::CardAccess::HAND>()[0], "Card1");
    EXPECT_EQ(player.get<server::CardAccess::HAND>()[1], "Card3");

    // Discard pile should have "Card2"
    ASSERT_EQ(player.get<server::CardAccess::DISCARD_PILE>().size(), 1);
    EXPECT_EQ(player.get<server::CardAccess::DISCARD_PILE>()[0], "Card2");
}

TEST(PlayerTest, AddCard)
{
    server::Player player("player");
    std::vector<std::string> hand = {"Card1", "Card2"};
    player.get<server::CardAccess::HAND>() = hand;

    // Add "Card3" to hand
    player.add<server::CardAccess::HAND>("Card3");

    // Now hand should have "Card1", "Card2", "Card3"
    ASSERT_EQ(player.get<server::CardAccess::HAND>().size(), 3);
    EXPECT_EQ(player.get<server::CardAccess::HAND>()[2], "Card3");
}

TEST(PlayerTest, AddToDiscardPile)
{
    server::Player player("player");

    // Discard pile is initially empty
    EXPECT_TRUE(player.get<server::CardAccess::DISCARD_PILE>().empty());

    // Add "Card1" to discard pile
    player.add<server::CardAccess::DISCARD_PILE>("Card1");

    // Now discard pile should have "Card1"
    ASSERT_EQ(player.get<server::CardAccess::DISCARD_PILE>().size(), 1);
    EXPECT_EQ(player.get<server::CardAccess::DISCARD_PILE>()[0], "Card1");
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
    player.get<server::CardAccess::HAND>() = {"Card1", "Card2"};
    player.get<server::CardAccess::PLAYED_CARDS>() = {"Card3"};
    player.get<server::CardAccess::DISCARD_PILE>() = {"Card4"};

    // Call end_turn()
    player.end_turn();

    // Check that values are reset
    EXPECT_EQ(player.getActions(), 1);
    EXPECT_EQ(player.getBuys(), 1);
    EXPECT_EQ(player.getTreasure(), 0);

    std::cerr << "hand: " << player.get<server::CardAccess::HAND>().size() << std::endl;
    std::cerr << "played: " << player.get<server::CardAccess::PLAYED_CARDS>().size() << std::endl;
    std::cerr << "discard: " << player.get<server::CardAccess::DISCARD_PILE>().size() << std::endl;
    std::cerr << "draw: " << player.get<server::CardAccess::DRAW_PILE_TOP>().size() << std::endl;
    // hand should contain 5 cards again
    EXPECT_EQ(player.get<server::CardAccess::HAND>().size(), 4);

    // discard pile should be empty as reshuffle was triggered
    ASSERT_EQ(player.get<server::CardAccess::DISCARD_PILE>().size(), 0); // hand + played + pile
}


TEST(PlayerTest, ShufflePile)
{
    server::Player player("player");

    // Set up discard pile
    player.get<server::CardAccess::DISCARD_PILE>() = {"Card1", "Card2", "Card3", "Card4", "Card5"};

    // Call shuffle on the discard pile
    player.shuffle<server::DISCARD_PILE>();

    // Ensure the pile still contains the same cards
    auto &shuffled_pile = player.get<server::CardAccess::DISCARD_PILE>();
    ASSERT_EQ(shuffled_pile.size(), 5);

    // Verify that the order is not guaranteed to be the same
    EXPECT_THAT(shuffled_pile, ::testing::UnorderedElementsAre("Card1", "Card2", "Card3", "Card4", "Card5"));
}


TEST(PlayerTest, TakeCards)
{
    server::Player player("player");

    // Set up draw pile
    player.get<server::CardAccess::DRAW_PILE_TOP>() = {"Card1", "Card2", "Card3"};

    // Take 2 cards
    auto taken_cards = player.take<server::DRAW_PILE_TOP>(2);

    // Verify the taken cards
    ASSERT_EQ(taken_cards.size(), 2);
    EXPECT_EQ(taken_cards[0], "Card1");
    EXPECT_EQ(taken_cards[1], "Card2");

    // Verify the remaining pile
    auto &draw_pile = player.get<server::CardAccess::DRAW_PILE_TOP>();
    ASSERT_EQ(draw_pile.size(), 1);
    EXPECT_EQ(draw_pile[0], "Card3");
}

TEST(PlayerTest, TakeWithReshuffle)
{
    server::Player player("player");

    // Set up discard pile and empty draw pile
    player.get<server::CardAccess::DISCARD_PILE>() = {"Card4", "Card5", "Card6"};
    player.get<server::CardAccess::DRAW_PILE_TOP>().clear();

    // Take 2 cards, forcing a reshuffle
    auto taken_cards = player.take<server::DRAW_PILE_TOP>(2);

    // Verify that reshuffle occurred
    ASSERT_EQ(taken_cards.size(), 2);

    // Ensure draw pile and discard pile are correctly updated
    EXPECT_EQ(player.get<server::CardAccess::DISCARD_PILE>().size(), 0);
    EXPECT_EQ(player.get<server::CardAccess::DRAW_PILE_TOP>().size(), 1); // 1 card left after taking 2
}


TEST(PlayerTest, AddCardsToPile)
{
    server::Player player("player");

    // Set up an empty discard pile
    player.get<server::CardAccess::DISCARD_PILE>().clear();

    // Add cards to the discard pile
    player.add<server::DISCARD_PILE>({"Card1", "Card2", "Card3"});

    // Verify the discard pile
    auto &discard_pile = player.get<server::CardAccess::DISCARD_PILE>();
    ASSERT_EQ(discard_pile.size(), 3);
    EXPECT_EQ(discard_pile[0], "Card1");
    EXPECT_EQ(discard_pile[1], "Card2");
    EXPECT_EQ(discard_pile[2], "Card3");
}

TEST(PlayerTest, MoveCardsBetweenPiles)
{
    server::Player player("player");

    // Set up hand and discard piles
    player.get<server::CardAccess::HAND>() = {"Card1", "Card2"};
    player.get<server::CardAccess::DISCARD_PILE>() = {"Card3"};

    // Move cards from hand to discard pile
    player.move<server::HAND, server::DISCARD_PILE>(2);

    // Verify the discard pile
    auto &discard_pile = player.get<server::CardAccess::DISCARD_PILE>();
    ASSERT_EQ(discard_pile.size(), 3);
    ASSERT_EQ(player.get<server::CardAccess::HAND>().size(), 0);
    EXPECT_EQ(discard_pile[0], "Card3");
    EXPECT_EQ(discard_pile[1], "Card1");
    EXPECT_EQ(discard_pile[2], "Card2");

    // Verify the hand is empty
    EXPECT_TRUE(player.get<server::CardAccess::HAND>().empty());
}

TEST(PlayerTest, GetPile)
{
    server::Player player("player");

    // Set up piles
    player.get<server::CardAccess::DISCARD_PILE>() = {"Card1", "Card2"};
    player.get<server::CardAccess::DRAW_PILE_TOP>() = {"Card3", "Card4"};
    player.get<server::CardAccess::HAND>() = {"Card5"};
    player.get<server::CardAccess::PLAYED_CARDS>() = {"Card6"};

    // Access and verify each pile
    EXPECT_EQ(player.get<server::DISCARD_PILE>()[0], "Card1");
    EXPECT_EQ(player.get<server::DISCARD_PILE>()[1], "Card2");

    EXPECT_EQ(player.get<server::DRAW_PILE_TOP>()[0], "Card3");
    EXPECT_EQ(player.get<server::DRAW_PILE_TOP>()[1], "Card4");

    EXPECT_EQ(player.get<server::HAND>()[0], "Card5");

    EXPECT_EQ(player.get<server::PLAYED_CARDS>()[0], "Card6");
}
