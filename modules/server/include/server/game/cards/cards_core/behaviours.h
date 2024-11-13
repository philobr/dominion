#pragma once

#include <string>
#include <tuple>
#include <utility>

#include <server/game/game_state/game_state.h>
#include <server/game/game_state/server_player.h>

namespace server
{
    // forward declaration
    class GameState;
    namespace behaviour
    {
        struct BehaviourBase
        {
            /**
             * @brief This function applies the specified behaviour onto a player
             *
             * @param gamestate
             * @param affected_player
             * @return true
             * @return false
             */
            virtual bool apply(server::GameState &gamestate, server::Player::id_t &affected_player) = 0;
        };

        template <int coins>
        struct GainCoins : public BehaviourBase
        {
            bool apply(server::GameState &gamestate, server::Player::id_t &affected_player);
        };

        template <int buys>
        struct GainBuys : public BehaviourBase
        {
            bool apply(server::GameState &gamestate, server::Player::id_t &affected_player);
        };

        template <int actions>
        struct GainActions : public BehaviourBase
        {
            bool apply(server::GameState &gamestate, server::Player::id_t &affected_player);
        };

        template <int points>
        struct GainPoints : public BehaviourBase
        {
            bool apply(server::GameState &gamestate, server::Player::id_t &affected_player);
        };
    } // namespace behaviour
} // namespace server

#include "behaviours_impl.hpp"