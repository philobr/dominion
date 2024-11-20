#pragma once

#include <vector>

#include <shared/game/game_state/board_base.h>
#include <shared/game/game_state/player_base.h>

namespace shared
{
    class ReducedGameState
    {
    public:
        // Constructor to use on the server side
        ReducedGameState(Board::ptr_t board, ReducedPlayer::ptr_t reduced_player,
                         std::vector<ReducedEnemy::ptr_t> reduced_enemies, const PlayerBase::id_t &active_player) :
            board(std::move(board)),
            reduced_player(std::move(reduced_player)), reduced_enemies(std::move(reduced_enemies)),
            active_player(active_player)
        {}

        ReducedGameState(ReducedGameState &&other) :
            board(std::move(other.board)), reduced_player(std::move(other.reduced_player)),
            reduced_enemies(std::move(other.reduced_enemies)), active_player(other.active_player)
        {}

        bool operator==(const ReducedGameState &other) const;
        bool operator!=(const ReducedGameState &other) const;

        /**
         * @brief Serialize the ReducedGameState to a JSON object.
         */
        rapidjson::Document toJson() const;
        /**
         * @brief Deserialize a ReducedGameState from a JSON object.
         */
        static std::unique_ptr<ReducedGameState> fromJson(const rapidjson::Value &json);

        Board::ptr_t board;
        ReducedPlayer::ptr_t reduced_player;
        std::vector<ReducedEnemy::ptr_t> reduced_enemies;
        PlayerBase::id_t active_player;
    };
} // namespace shared
