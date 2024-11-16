#pragma once

#include <server/game/cards/cards_core/behaviours.h>
#include <server/game/game_state/game_state.h>

namespace server
{
    namespace behaviour
    {
        static constexpr bool DONE = true;
        static constexpr bool NOT_DONE_YET = false;

        template <int coins>
        bool GainCoins<coins>::apply(server::GameState &gamestate, server::Player::id_t &affected_player)
        {
            gamestate.get_player(affected_player).incTreasure(coins);
            return DONE;
        }

        template <int buys>
        bool GainBuys<buys>::apply(server::GameState &gamestate, server::Player::id_t &affected_player)
        {
            gamestate.get_player(affected_player).incBuys(buys);
            return DONE;
        }

        template <int actions>
        bool GainActions<actions>::apply(server::GameState &gamestate, server::Player::id_t &affected_player)
        {
            gamestate.get_player(affected_player).incActions(actions);
            return DONE;
        }

        template <int points>
        bool GainPoints<points>::apply(server::GameState &gamestate, server::Player::id_t &affected_player)
        {
            gamestate.get_player(affected_player).incPoints(points);
            return DONE;
        }

    } // namespace behaviour
} // namespace server