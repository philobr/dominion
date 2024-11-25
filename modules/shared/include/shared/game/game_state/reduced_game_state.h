#pragma once

#include <vector>

#include <shared/game/game_state/board_base.h>
#include <shared/game/reduced/player.h>

namespace reduced
{
    class GameState
    {
    public:
        // Constructor to use on the server side
        GameState(shared::Board::ptr_t board, reduced::Player::ptr_t reduced_player,
                  std::vector<reduced::Enemy::ptr_t> &&reduced_enemies, const shared::PlayerBase::id_t &active_player) :
            board(std::move(board)),
            reduced_player(std::move(reduced_player)), reduced_enemies(std::move(reduced_enemies)),
            active_player(active_player)
        {}

        GameState(GameState &&other) :
            board(std::move(other.board)), reduced_player(std::move(other.reduced_player)),
            reduced_enemies(std::move(other.reduced_enemies)), active_player(other.active_player)
        {}

        bool operator==(const GameState &other) const;
        bool operator!=(const GameState &other) const;

        /**
         * @brief Serialize the GameState to a JSON object.
         */
        rapidjson::Document toJson() const;
        /**
         * @brief Deserialize a GameState from a JSON object.
         */
        static std::unique_ptr<GameState> fromJson(const rapidjson::Value &json);

        shared::Board::ptr_t board;
        reduced::Player::ptr_t reduced_player;
        std::vector<reduced::Enemy::ptr_t> reduced_enemies;
        shared::PlayerBase::id_t active_player;
    };
} // namespace reduced
