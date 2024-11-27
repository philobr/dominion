#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <server/game/game_state/server_player.h>
#include <shared/utils/test_helpers.h>

using namespace server;

using testing::ElementsAreArray;
using testing::UnorderedElementsAreArray;

class TestPlayer : public Player
{
public:
    using Player::Player;

    // To make setups easier
    using Player::getMutable;

    // Expose the functions to test
    using Player::add;
    using Player::get;
    using Player::move;
    using Player::move_indices;
    using Player::shuffle;
    using Player::take;
    using Player::take_indices;
};

TEST(PlayerTest, TakeFromDrawPileTop)
{
    TestPlayer player("Player");

    // Setup: Add cards to the draw pile top
    auto kingdom_cards = get_valid_kingdom_cards(); // Returns std::vector<std::string>
    player.add<DRAW_PILE_TOP>(kingdom_cards);

    // Take 3 cards from the top
    auto taken_cards = player.take<DRAW_PILE_TOP>(3);

    // Verify the taken cards are the first 3 cards
    ASSERT_EQ(taken_cards.size(), 3);
    EXPECT_EQ(taken_cards[0], kingdom_cards[0]);
    EXPECT_EQ(taken_cards[1], kingdom_cards[1]);
    EXPECT_EQ(taken_cards[2], kingdom_cards[2]);

    // Verify the remaining draw pile
    const auto &draw_pile = player.get<DRAW_PILE_TOP>();
    ASSERT_EQ(draw_pile.size(), kingdom_cards.size() - 3);
    for ( size_t i = 0; i < draw_pile.size(); ++i ) {
        EXPECT_EQ(draw_pile[i], kingdom_cards[i + 3]);
    }
}

TEST(PlayerTest, TakeFromDrawPileBottom)
{
    TestPlayer player("Player");

    // Setup: Add cards to the draw pile bottom
    auto kingdom_cards = get_valid_kingdom_cards();
    player.add<DRAW_PILE_BOTTOM>(kingdom_cards);

    // Take 2 cards from the bottom
    auto taken_cards = player.take<DRAW_PILE_BOTTOM>(2);

    // Verify the taken cards are the last 2 cards
    ASSERT_EQ(taken_cards.size(), 2);
    EXPECT_EQ(taken_cards[0], kingdom_cards[kingdom_cards.size() - 2]);
    EXPECT_EQ(taken_cards[1], kingdom_cards[kingdom_cards.size() - 1]);

    // Verify the remaining draw pile
    const auto &draw_pile = player.get<DRAW_PILE_BOTTOM>();
    ASSERT_EQ(draw_pile.size(), kingdom_cards.size() - 2);
    for ( size_t i = 0; i < draw_pile.size(); ++i ) {
        EXPECT_EQ(draw_pile[i], kingdom_cards[i]);
    }
}

TEST(PlayerTest, TakeFromHand)
{
    TestPlayer player("Player");

    // Setup: Add cards to hand
    auto kingdom_cards = get_valid_kingdom_cards();
    player.add<HAND>(kingdom_cards);

    // Take 5 cards from the hand
    auto taken_cards = player.take<HAND>(5);

    // Verify the taken cards
    ASSERT_EQ(taken_cards.size(), 5);
    for ( size_t i = 0; i < 5; ++i ) {
        EXPECT_EQ(taken_cards[i], kingdom_cards[kingdom_cards.size() - 1 - i]);
    }

    // Verify the remaining hand
    const auto &hand = player.get<HAND>();
    ASSERT_EQ(hand.size(), kingdom_cards.size() - 5);
    for ( size_t i = 0; i < hand.size(); ++i ) {
        EXPECT_EQ(hand[i], kingdom_cards[i]);
    }
}

TEST(PlayerTest, AddToHand)
{
    TestPlayer player("Player");

    // Setup: Get some cards
    auto kingdom_cards = get_valid_kingdom_cards();

    // Add cards to hand
    player.add<HAND>(kingdom_cards);

    // Verify that hand has these cards
    const auto &hand = player.get<HAND>();
    ASSERT_EQ(hand.size(), kingdom_cards.size());
    EXPECT_THAT(hand, ElementsAreArray(kingdom_cards));
}

TEST(PlayerTest, AddToDrawPileTop)
{
    TestPlayer player("Player");

    // Setup: Get some cards
    auto kingdom_cards = get_valid_kingdom_cards();
    std::vector<std::string> first_half(kingdom_cards.begin(), kingdom_cards.begin() + 5);
    std::vector<std::string> second_half(kingdom_cards.begin() + 5, kingdom_cards.end());

    // Add first half to draw pile
    player.add<DRAW_PILE_TOP>(first_half);

    // Add second half to top of draw pile
    player.add<DRAW_PILE_TOP>(second_half);

    // Verify draw pile order (second_half followed by first_half)
    const auto &draw_pile = player.get<DRAW_PILE_TOP>();
    ASSERT_EQ(draw_pile.size(), kingdom_cards.size());
    std::vector<std::string> expected_draw_pile;
    expected_draw_pile.insert(expected_draw_pile.end(), second_half.begin(), second_half.end());
    expected_draw_pile.insert(expected_draw_pile.end(), first_half.begin(), first_half.end());

    EXPECT_EQ(draw_pile, expected_draw_pile);
}

TEST(PlayerTest, AddToDrawPileBottom)
{
    TestPlayer player("Player");

    // Setup: Get some cards
    auto kingdom_cards = get_valid_kingdom_cards();

    // Add cards to the bottom of the draw pile
    player.add<DRAW_PILE_BOTTOM>(kingdom_cards);

    // Verify draw pile order
    const auto &draw_pile = player.get<DRAW_PILE_BOTTOM>();
    ASSERT_EQ(draw_pile.size(), kingdom_cards.size());
    EXPECT_EQ(draw_pile, kingdom_cards);
}

TEST(PlayerTest, MoveFromHandToDiscardPile)
{
    TestPlayer player("Player");

    // Setup: Add cards to hand
    auto kingdom_cards = get_valid_kingdom_cards();
    player.add<HAND>(kingdom_cards);

    // Move 2 cards from hand to discard pile
    player.move<HAND, DISCARD_PILE>(2);

    // Verify hand has remaining cards
    const auto &hand = player.get<HAND>();
    ASSERT_EQ(hand.size(), kingdom_cards.size() - 2);
    for ( size_t i = 0; i < hand.size(); ++i ) {
        EXPECT_EQ(hand[i], kingdom_cards[i]);
    }

    // Verify discard pile has the moved cards
    const auto &discard_pile = player.get<DISCARD_PILE>();
    ASSERT_EQ(discard_pile.size(), 2);
    EXPECT_EQ(discard_pile[0], kingdom_cards[kingdom_cards.size() - 2]);
    EXPECT_EQ(discard_pile[1], kingdom_cards[kingdom_cards.size() - 1]);
}

TEST(PlayerTest, MoveIndicesFromHandToDiscardPile)
{
    TestPlayer player("Player");

    // Setup: Add cards to hand
    auto kingdom_cards = get_valid_kingdom_cards();
    player.add<HAND>(kingdom_cards);

    // Indices to move
    std::vector<unsigned int> indices = {0, 2, 4};

    // Move cards at indices from hand to discard pile
    player.move_indices<HAND, DISCARD_PILE>(indices);

    // Verify discard pile has the moved cards
    const auto &discard_pile = player.get<DISCARD_PILE>();
    ASSERT_EQ(discard_pile.size(), indices.size());
    for ( size_t i = 0; i < indices.size(); ++i ) {
        EXPECT_EQ(discard_pile[i], kingdom_cards[indices[i]]);
    }

    // Verify remaining hand
    const auto &hand = player.get<HAND>();
    ASSERT_EQ(hand.size(), kingdom_cards.size() - indices.size());

    // Remove moved cards from original kingdom_cards to compare
    std::vector<std::string> expected_hand = kingdom_cards;
    std::sort(indices.rbegin(), indices.rend());
    for ( auto idx : indices ) {
        expected_hand.erase(expected_hand.begin() + idx);
    }

    EXPECT_EQ(hand, expected_hand);
}

TEST(PlayerTest, TakeIndicesFromHand)
{
    TestPlayer player("Player");

    // Setup: Add cards to hand
    auto kingdom_cards = get_valid_kingdom_cards();
    player.add<HAND>(kingdom_cards);

    // Indices to take
    std::vector<unsigned int> indices = {1, 3, 5};

    // Take cards at indices
    auto taken_cards = player.take_indices<HAND>(indices);

    // Verify taken cards
    ASSERT_EQ(taken_cards.size(), indices.size());
    for ( size_t i = 0; i < indices.size(); ++i ) {
        EXPECT_EQ(taken_cards[i], kingdom_cards[indices[i]]);
    }

    // Verify remaining hand
    const auto &hand = player.get<HAND>();
    ASSERT_EQ(hand.size(), kingdom_cards.size() - indices.size());

    // Remove taken cards from original kingdom_cards to compare
    std::vector<std::string> expected_hand = kingdom_cards;
    std::sort(indices.rbegin(), indices.rend());
    for ( auto idx : indices ) {
        expected_hand.erase(expected_hand.begin() + idx);
    }

    EXPECT_EQ(hand, expected_hand);
}

TEST(PlayerTest, TakeMoreCardsThanAvailable)
{
    TestPlayer player("Player");

    // Setup: Add a few cards to hand
    auto kingdom_cards = get_valid_kingdom_cards();
    std::vector<std::string> few_cards(kingdom_cards.begin(), kingdom_cards.begin() + 2);
    player.add<HAND>(few_cards);

    // Attempt to take more cards than available
    EXPECT_THROW({ player.take<HAND>(3); }, std::runtime_error);
}

TEST(PlayerTest, TakeFromEmptyDrawPileShufflesDiscardPile)
{
    TestPlayer player("Player");

    // Setup: discard pile has cards, draw pile is empty
    auto kingdom_cards = get_valid_kingdom_cards();
    player.add<DISCARD_PILE>(kingdom_cards);

    // Draw 3 cards, which should shuffle discard pile into draw pile
    player.draw(3);

    // Verify hand has 3 cards
    const auto &hand = player.get<HAND>();
    ASSERT_EQ(hand.size(), 3);

    // Verify discard pile is empty
    const auto &discard_pile = player.get<DISCARD_PILE>();
    EXPECT_TRUE(discard_pile.empty());

    // Verify draw pile has remaining cards
    const auto &draw_pile = player.get<DRAW_PILE_TOP>();
    ASSERT_EQ(draw_pile.size(), kingdom_cards.size() - 3);

    // Since the discard pile was shuffled into draw pile, we cannot predict the order,
    // but we can check that all cards are accounted for.
    std::vector<std::string> all_cards;
    all_cards.insert(all_cards.end(), hand.begin(), hand.end());
    all_cards.insert(all_cards.end(), draw_pile.begin(), draw_pile.end());
    EXPECT_THAT(all_cards, UnorderedElementsAreArray(kingdom_cards));
}

TEST(PlayerTest, MoveFromHandToTrash)
{
    TestPlayer player("Player");

    // Setup: Add cards to hand
    auto kingdom_cards = get_valid_kingdom_cards();
    player.add<HAND>(kingdom_cards);

    // Move 2 cards from hand to trash
    player.move<HAND, TRASH>(2);

    // Verify hand has remaining cards
    const auto &hand = player.get<HAND>();
    ASSERT_EQ(hand.size(), kingdom_cards.size() - 2);

    // Verify discard pile is unaffected
    const auto &discard_pile = player.get<DISCARD_PILE>();
    EXPECT_TRUE(discard_pile.empty());

    // Verify draw pile is unaffected
    const auto &draw_pile = player.get<DRAW_PILE_TOP>();
    EXPECT_TRUE(draw_pile.empty());
}

TEST(PlayerTest, MoveFromTrashThrows)
{
    TestPlayer player("Player");

    // Attempt to move cards from the trash
    EXPECT_THROW((player.move<TRASH, HAND>(1)), std::runtime_error);
}

TEST(PlayerTest, AddSingleCardToHand)
{
    TestPlayer player("Player");

    // Setup: Get a single card
    auto kingdom_cards = get_valid_kingdom_cards();
    std::string card_id = kingdom_cards[0];

    // Add single card to hand
    player.add<HAND>(card_id);

    // Verify hand has the card
    const auto &hand = player.get<HAND>();
    ASSERT_EQ(hand.size(), 1);
    EXPECT_EQ(hand[0], card_id);
}

TEST(PlayerTest, AddSingleCardToDrawPileTop)
{
    TestPlayer player("Player");

    // Setup: Get some cards
    auto kingdom_cards = get_valid_kingdom_cards();
    std::string card_id = kingdom_cards[0];

    // Add a card to draw pile top
    player.add<DRAW_PILE_TOP>(card_id);

    // Verify draw pile has the card
    const auto &draw_pile = player.get<DRAW_PILE_TOP>();
    ASSERT_EQ(draw_pile.size(), 1);
    EXPECT_EQ(draw_pile[0], card_id);
}

TEST(PlayerTest, ShuffleDrawPile)
{
    TestPlayer player("Player");

    // Setup: Add cards to draw pile
    auto kingdom_cards = get_valid_kingdom_cards();
    player.add<DRAW_PILE_TOP>(kingdom_cards);

    // Shuffle the draw pile
    player.shuffle<DRAW_PILE_TOP>();

    // Verify the draw pile has the same cards but in a different order
    const auto &draw_pile = player.get<DRAW_PILE_TOP>();
    ASSERT_EQ(draw_pile.size(), kingdom_cards.size());
    EXPECT_THAT(draw_pile, UnorderedElementsAreArray(kingdom_cards));
    EXPECT_FALSE(std::equal(draw_pile.begin(), draw_pile.end(), kingdom_cards.begin()));
}
