#pragma once

#include <memory>
#include <string>
namespace shared
{
    // Representing card types like this allows us to combine them using bitwise operations.
    // Example: `ACTION | ATTACK` will create a card that is both an action and an attack.
    enum CardType
    {
        KINGDOM = 1,
        TREASURE = 2,
        VICTORY = 4,
        CURSE = 8 | VICTORY,


        ACTION = 16 | KINGDOM,
        ATTACK = 32 | ACTION,
        REACTION = 64 | ACTION
    };

    class CardBase
    {
    public:
        using id_t = std::string;
        using ptr_t = std::unique_ptr<CardBase>;

        CardBase(id_t id, CardType type, unsigned int cost) : id(id), type(type), cost(cost) {}

        unsigned int getCost() const { return cost; }
        CardType getType() const { return type; }
        id_t getId() const { return id; }

        bool isAction() const { return (type & ACTION) == ACTION; }
        bool isAttack() const { return (type & ATTACK) == ATTACK; }
        bool isTreasure() const { return (type & TREASURE) == TREASURE; }
        bool isReaction() const { return (type & REACTION) == REACTION; }
        bool isVictory() const { return (type & VICTORY) == VICTORY; }
        bool isCurse() const { return (type & CURSE) == CURSE; }
        bool isKingdom() const { return (type & KINGDOM) == KINGDOM; }

    protected:
        id_t id;
        const CardType type;
        const unsigned int cost;
    };

} // namespace shared
