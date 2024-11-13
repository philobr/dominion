#pragma once

#include <vector>

#include <shared/game/game_state/board_base.h>
namespace server

{
    // forward declarations
    class CardBase;
    class GameState;

    class ServerBoard : public shared::Board
    {
    public:
        ServerBoard(const std::vector<shared::CardBase::id_t> &kingdom_cards, size_t player_count);
        ~ServerBoard() = default;

        bool buy(const shared::CardBase::id_t &card);
        void trash_card(const shared::CardBase::id_t &card);
    };

} // namespace server