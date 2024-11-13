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
    } // namespace behaviour
} // namespace server

#include "behaviours_impl.hpp"