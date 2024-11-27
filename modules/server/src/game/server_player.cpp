#include <algorithm>
#include <random>
#include <ranges>

#include <server/game/server_player.h>
#include <shared/utils/assert.h>

namespace server
{
    reduced::Player::ptr_t Player::getReducedPlayer()
    {
        return reduced::Player::make(static_cast<shared::PlayerBase>(*this), hand_cards);
    }

    reduced::Enemy::ptr_t Player::getReducedEnemy()
    {
        return reduced::Enemy::make(static_cast<shared::PlayerBase>(*this), hand_cards.size());
    }

    void Player::resetValues()
    {
        actions = 1;
        buys = 1;
        treasure = 0;
        victory_points = 0;
    }

    void Player::endTurn()
    {
        resetValues();

        move<HAND, DISCARD_PILE>();
        move<PLAYED_CARDS, DISCARD_PILE>();

        draw(5);
    }

} // namespace server
