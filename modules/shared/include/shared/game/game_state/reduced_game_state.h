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
                         std::vector<ReducedEnemy::ptr_t> &&reduced_enemies,
                         const PlayerBase::id_t &active_player) :
            board(std::move(board)),
            reduced_player(std::move(reduced_player)), reduced_enemies(std::move(reduced_enemies)),
            active_player(active_player)
        {}

        rapidjson::Document toJson() const;
        ReducedPlayer fromJson(const rapidjson::Value &json);

        Board::ptr_t board;
        const ReducedPlayer::ptr_t reduced_player;
        const std::vector<ReducedEnemy::ptr_t> reduced_enemies;
        PlayerBase::id_t active_player;
    };
} // namespace shared
