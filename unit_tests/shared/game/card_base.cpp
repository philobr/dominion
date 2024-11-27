#include <gtest/gtest.h>
#include <shared/game/cards/card_base.h>

TEST(CardBaseTest, ConstructorAndGetters)
{
    // Create a card with a single type
    shared::CardBase::id_t card_id = "Village";
    shared::CardType card_type = shared::ACTION;
    unsigned int card_cost = 3;

    shared::CardBase card(card_id, card_type, card_cost);

    EXPECT_EQ(card.getId(), card_id);
    EXPECT_EQ(card.getCost(), card_cost);
    EXPECT_EQ(card.getType(), card_type);
}

TEST(CardBaseTest, IsTypeMethodsSingleType)
{
    // Create a Treasure card
    shared::CardBase::id_t card_id = "Gold";
    shared::CardType card_type = shared::TREASURE;
    unsigned int card_cost = 6;

    shared::CardBase card(card_id, card_type, card_cost);

    EXPECT_FALSE(card.isAction());
    EXPECT_FALSE(card.isAttack());
    EXPECT_TRUE(card.isTreasure());
    EXPECT_FALSE(card.isReaction());
    EXPECT_FALSE(card.isVictory());
    EXPECT_FALSE(card.isCurse());
}

TEST(CardBaseTest, IsTypeMethodsMultipleTypes)
{
    // Create a card with multiple types (e.g., Action and Reaction)
    shared::CardBase::id_t card_id = "Moat";
    shared::CardType card_type = static_cast<shared::CardType>(shared::ACTION | shared::REACTION);
    unsigned int card_cost = 2;

    shared::CardBase card(card_id, card_type, card_cost);

    EXPECT_TRUE(card.isAction());
    EXPECT_FALSE(card.isAttack());
    EXPECT_FALSE(card.isTreasure());
    EXPECT_TRUE(card.isReaction());
    EXPECT_FALSE(card.isVictory());
    EXPECT_FALSE(card.isCurse());
}

TEST(CardBaseTest, IsAttackMethod)
{
    // Create an Attack card
    shared::CardBase::id_t card_id = "Militia";
    shared::CardType card_type = static_cast<shared::CardType>(shared::ACTION | shared::ATTACK);
    unsigned int card_cost = 4;

    shared::CardBase card(card_id, card_type, card_cost);

    EXPECT_TRUE(card.isAction());
    EXPECT_TRUE(card.isAttack());
    EXPECT_FALSE(card.isTreasure());
    EXPECT_FALSE(card.isReaction());
    EXPECT_FALSE(card.isVictory());
    EXPECT_FALSE(card.isCurse());
}

TEST(CardBaseTest, IsVictoryMethod)
{
    // Create a Victory card
    shared::CardBase::id_t card_id = "Province";
    shared::CardType card_type = shared::VICTORY;
    unsigned int card_cost = 8;

    shared::CardBase card(card_id, card_type, card_cost);

    EXPECT_FALSE(card.isAction());
    EXPECT_FALSE(card.isAttack());
    EXPECT_FALSE(card.isTreasure());
    EXPECT_FALSE(card.isReaction());
    EXPECT_TRUE(card.isVictory());
    EXPECT_FALSE(card.isCurse());
}

TEST(CardBaseTest, IsCurseMethod)
{
    // Create a Curse card
    shared::CardBase::id_t card_id = "Curse";
    shared::CardType card_type = shared::CURSE;
    unsigned int card_cost = 0;

    shared::CardBase card(card_id, card_type, card_cost);

    EXPECT_FALSE(card.isAction());
    EXPECT_FALSE(card.isAttack());
    EXPECT_FALSE(card.isTreasure());
    EXPECT_FALSE(card.isReaction());
    EXPECT_FALSE(card.isVictory());
    EXPECT_TRUE(card.isCurse());
}

TEST(CardBaseTest, MultipleTypes)
{
    // Create a card that is both an Action and an Attack
    shared::CardBase::id_t card_id = "Witch";
    shared::CardType card_type = static_cast<shared::CardType>(shared::ACTION | shared::ATTACK);
    unsigned int card_cost = 5;

    shared::CardBase card(card_id, card_type, card_cost);

    EXPECT_TRUE(card.isAction());
    EXPECT_TRUE(card.isAttack());
    EXPECT_FALSE(card.isTreasure());
    EXPECT_FALSE(card.isReaction());
    EXPECT_FALSE(card.isVictory());
    EXPECT_FALSE(card.isCurse());
}

TEST(CardBaseTest, AllTypes)
{
    // Create a card that has all types (unlikely in game but for testing purposes)
    shared::CardBase::id_t card_id = "TestCard";
    shared::CardType card_type = static_cast<shared::CardType>(shared::ACTION | shared::ATTACK | shared::REACTION |
                                                               shared::TREASURE | shared::VICTORY | shared::CURSE);
    unsigned int card_cost = 10;

    shared::CardBase card(card_id, card_type, card_cost);

    EXPECT_TRUE(card.isAction());
    EXPECT_TRUE(card.isAttack());
    EXPECT_TRUE(card.isTreasure());
    EXPECT_TRUE(card.isReaction());
    EXPECT_TRUE(card.isVictory());
    EXPECT_TRUE(card.isCurse());
}

TEST(CardBaseTest, NoType)
{
    // Create a card with no type (edge case)
    shared::CardBase::id_t card_id = "Blank";
    shared::CardType card_type = static_cast<shared::CardType>(0);
    unsigned int card_cost = 0;

    shared::CardBase card(card_id, card_type, card_cost);

    EXPECT_FALSE(card.isAction());
    EXPECT_FALSE(card.isAttack());
    EXPECT_FALSE(card.isTreasure());
    EXPECT_FALSE(card.isReaction());
    EXPECT_FALSE(card.isVictory());
    EXPECT_FALSE(card.isCurse());
}

TEST(CardBaseTest, CostTest)
{
    // Test various costs
    shared::CardBase card1("Copper", shared::TREASURE, 0);
    shared::CardBase card2("Silver", shared::TREASURE, 3);
    shared::CardBase card3("Gold", shared::TREASURE, 6);

    EXPECT_EQ(card1.getCost(), 0);
    EXPECT_EQ(card2.getCost(), 3);
    EXPECT_EQ(card3.getCost(), 6);
}
