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
                         std::vector<ReducedEnemy::ptr_t> &&reduced_enemies, // rvalue ref
                         const PlayerBase::id_t &active_player) :
            board(std::move(board)),
            reduced_player(std::move(reduced_player)), reduced_enemies(std::move(reduced_enemies)),
            active_player(active_player)
        {}

        std::string toJson() const;
        ReducedPlayer fromJson(const std::string &json);
        std::unique_ptr<ReducedPlayer> uniuqeFromJson(const std::string &json);

        Board::ptr_t board;
        const ReducedPlayer::ptr_t reduced_player;
        const std::vector<ReducedEnemy::ptr_t> reduced_enemies;
        PlayerBase::id_t active_player;
    };
} // namespace shared