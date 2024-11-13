#pragma once

#include <memory>
#include <string>
namespace shared
{
    // Representing card types like this allows us to combine them using bitwise operations.
    // Example: `ACTION | ATTACK` will create a card that is both an action and an attack.
    enum CardType
    {
        ACTION = 0b1,
        ATTACK = 0b10,
        REACTION = 0b100,
        TREASURE = 0b1000,
        VICTORY = 0b10000,
        CURSE = 0b100000
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

        bool isAction() const { return type & ACTION; }
        bool isAttack() const { return type & ATTACK; }
        bool isTreasure() const { return type & TREASURE; }
        bool isReaction() const { return type & REACTION; }
        bool isVictory() const { return type & VICTORY; }
        bool isCurse() const { return type & CURSE; }

    protected:
        id_t id;
        const CardType type;
        const unsigned int cost;
    };

} // namespace shared