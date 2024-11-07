#include <shared/game_state.h>

namespace shared
{
    CardBase::CardBase(id_t value, CardType type, unsigned int cost) : value(value), type(type), cost(cost) {}

    bool CardBase::isAction() const
    {
        return type & ACTION;
    }

    bool CardBase::isAttack() const
    {
        return type & ATTACK;
    }

    bool CardBase::isTreasure() const
    {
        return type & TREASURE;
    }

    bool CardBase::isReaction() const
    {
        return type & REACTION;
    }

    bool CardBase::isVictory() const
    {
        return type & VICTORY;
    }

    bool CardBase::isCurse() const
    {
        return type & CURSE;
    }

    unsigned int CardBase::getCost()
    {
        return cost;
    }

    CardType CardBase::getType()
    {
        return type;
    }

    CardBase::id_t CardBase::getId()
    {
        return value;
    }

    // TODO: Implement PlayerBase
        // ask Nicola why some stuff is protected. ID cannot be accessed by the game logic
        // i think it should be private/accessible by the game logic
    // Maybe functions for get_reduced_player in playerBase etc...
    
    PlayerBase::id_t PlayerBase::id() const
    {
        return player_id;
    }

    
} // namespace shared
