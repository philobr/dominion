#pragma once

#include <memory>
#include <string>
namespace shared
{
    // Representing card types like this allows us to combine them using bitwise operations.
    // Example: `ACTION | ATTACK` will create a card that is both an action and an attack.
    enum CardType
    {
        ACTION = 0b000001,
        ATTACK = 0b000010,
        REACTION = 0b000100,
        TREASURE = 0b001000,
        VICTORY = 0b010000,
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

        bool isAction() const { return (type & ACTION) != 0; }
        bool isAttack() const { return (type & ATTACK) != 0; }
        bool isTreasure() const { return (type & TREASURE) != 0; }
        bool isReaction() const { return (type & REACTION) != 0; }
        bool isVictory() const { return (type & VICTORY) != 0; }
        bool isCurse() const { return (type & CURSE) != 0; }

    protected:
        id_t id;
        const CardType type;
        const unsigned int cost;
    };

} // namespace shared
