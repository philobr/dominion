#include <algorithm>
#include <random>
#include <ranges>

#include <server/game/game_state/server_player.h>
#include <shared/utils/assert.h>

namespace server
{
    reduced::Player::ptr_t Player::get_reduced_player()
    {
        return reduced::Player::make(static_cast<shared::PlayerBase>(*this), hand_cards);
    }

    reduced::Enemy::ptr_t Player::get_reduced_enemy()
    {
        return reduced::Enemy::make(static_cast<shared::PlayerBase>(*this), hand_cards.size());
    }

    void Player::reset_values()
    {
        actions = 1;
        buys = 1;
        treasure = 0;
        victory_points = 0;
    }

    void Player::end_turn()
    {
        reset_values();

        move<HAND, DISCARD_PILE>();
        move<PLAYED_CARDS, DISCARD_PILE>();

        draw(5);
    }

} // namespace server
