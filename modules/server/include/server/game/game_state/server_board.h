#pragma once

#include <vector>

#include <shared/game/game_state/board_base.h>
namespace server

{
    // forward declarations
    class CardBase;
    class GameState;

    /**
     * @brief Implements the server board. One can buy a card based on id, or trash a card based on id.
     */
    class ServerBoard : public shared::Board
    {
    public:
        /**
         * @brief Constructs a ServerBoard for a given number of players and 10 kingdom cards.
         *
         * @param kingdom_cards 10 (this gets asserted!) kingdom cards
         * @param player_count
         */
        ServerBoard(const std::vector<shared::CardBase::id_t> &kingdom_cards, size_t player_count);

        ~ServerBoard() = default; // TODO: do we need more than this?

        /**
         * @brief Tries to buy a card based on id.
         *
         * @param card
         * @return true successfully bought the card
         * @return false card_id can not be bought
         */
        bool buy(const shared::CardBase::id_t &card_id);

        /**
         * @brief Adds the card_id to the trash
         * @param card
         */
        void trash_card(const shared::CardBase::id_t &card_id);
    };

} // namespace server