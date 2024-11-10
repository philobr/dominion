#include "shared/game_state.h"
#include <gtest/gtest.h>
using namespace shared;

class TestCard : public CardBase
{
public:
    TestCard(id_t value, CardType type, unsigned int cost) : CardBase(value, type, cost) {}

    std::string toString() override { return id; }

    virtual ~TestCard() = default;
};

class CardBaseTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Create test cards with different types
        actionCard = new TestCard("Action1", CardType::ACTION, 3);
        actionAttackCard = new TestCard("AttackAction1", static_cast<CardType>(CardType::ACTION | CardType::ATTACK), 4);
        treasureCard = new TestCard("Treasure1", CardType::TREASURE, 2);
        reactionCard = new TestCard("Reaction1", CardType::REACTION, 2);
        victoryCard = new TestCard("Victory1", CardType::VICTORY, 5);
        curseCard = new TestCard("Curse1", CardType::CURSE, 0);
    }

    void TearDown() override
    {
        delete actionCard;
        delete actionAttackCard;
        delete treasureCard;
        delete reactionCard;
        delete victoryCard;
        delete curseCard;
    }

    TestCard* actionCard;
    TestCard* actionAttackCard;
    TestCard* treasureCard;
    TestCard* reactionCard;
    TestCard* victoryCard;
    TestCard* curseCard;
};

// Test card type checks
TEST_F(CardBaseTest, IsAction)
{
    EXPECT_TRUE(actionCard->isAction());
    EXPECT_TRUE(actionAttackCard->isAction()); // Should be true for combined type
    EXPECT_FALSE(treasureCard->isAction());
}

TEST_F(CardBaseTest, IsAttack)
{
    EXPECT_TRUE(actionAttackCard->isAttack());
    EXPECT_FALSE(actionCard->isAttack());
    EXPECT_FALSE(treasureCard->isAttack());
}

TEST_F(CardBaseTest, IsTreasure)
{
    EXPECT_TRUE(treasureCard->isTreasure());
    EXPECT_FALSE(actionCard->isTreasure());
    EXPECT_FALSE(actionAttackCard->isTreasure());
}

TEST_F(CardBaseTest, IsReaction)
{
    EXPECT_TRUE(reactionCard->isReaction());
    EXPECT_FALSE(actionCard->isReaction());
    EXPECT_FALSE(treasureCard->isReaction());
}

TEST_F(CardBaseTest, IsVictory)
{
    EXPECT_TRUE(victoryCard->isVictory());
    EXPECT_FALSE(actionCard->isVictory());
    EXPECT_FALSE(treasureCard->isVictory());
}

TEST_F(CardBaseTest, IsCurse)
{
    EXPECT_TRUE(curseCard->isCurse());
    EXPECT_FALSE(actionCard->isCurse());
    EXPECT_FALSE(victoryCard->isCurse());
}

// Test combined types
TEST_F(CardBaseTest, CombinedTypes)
{
    // Create a card with multiple types
    TestCard multiCard("Multi", static_cast<CardType>(CardType::ACTION | CardType::ATTACK | CardType::REACTION), 5);

    EXPECT_TRUE(multiCard.isAction());
    EXPECT_TRUE(multiCard.isAttack());
    EXPECT_TRUE(multiCard.isReaction());
    EXPECT_FALSE(multiCard.isTreasure());
    EXPECT_FALSE(multiCard.isVictory());
    EXPECT_FALSE(multiCard.isCurse());
}

// Test getters
TEST_F(CardBaseTest, GetCost)
{
    EXPECT_EQ(actionCard->getCost(), 3);
    EXPECT_EQ(actionAttackCard->getCost(), 4);
    EXPECT_EQ(treasureCard->getCost(), 2);
    EXPECT_EQ(curseCard->getCost(), 0);
}

TEST_F(CardBaseTest, GetType)
{
    EXPECT_EQ(actionCard->getType(), CardType::ACTION);
    EXPECT_EQ(actionAttackCard->getType(), static_cast<CardType>(CardType::ACTION | CardType::ATTACK));
    EXPECT_EQ(treasureCard->getType(), CardType::TREASURE);
    EXPECT_EQ(victoryCard->getType(), CardType::VICTORY);
}

TEST_F(CardBaseTest, GetId)
{
    EXPECT_EQ(actionCard->getId(), "Action1");
    EXPECT_EQ(actionAttackCard->getId(), "AttackAction1");
    EXPECT_EQ(treasureCard->getId(), "Treasure1");
}

TEST_F(CardBaseTest, ToString)
{
    EXPECT_EQ(actionCard->toString(), "Action1");
    EXPECT_EQ(actionAttackCard->toString(), "AttackAction1");
    EXPECT_EQ(treasureCard->toString(), "Treasure1");
}

// Test constructor with different type combinations
TEST_F(CardBaseTest, ConstructorWithDifferentTypes)
{
    // Single type
    TestCard singleType("Test1", CardType::ACTION, 5);
    EXPECT_TRUE(singleType.isAction());
    EXPECT_FALSE(singleType.isAttack());

    // Double type
    TestCard doubleType("Test2", static_cast<CardType>(CardType::ACTION | CardType::ATTACK), 5);
    EXPECT_TRUE(doubleType.isAction());
    EXPECT_TRUE(doubleType.isAttack());

    // Triple type
    TestCard tripleType("Test3", static_cast<CardType>(CardType::ACTION | CardType::ATTACK | CardType::REACTION), 5);
    EXPECT_TRUE(tripleType.isAction());
    EXPECT_TRUE(tripleType.isAttack());
    EXPECT_TRUE(tripleType.isReaction());
}
