#pragma once

#include <vector>

#include <shared/game/game_state/board_base.h>
#include <shared/utils/assert.h>
namespace server
{

    /**
     * @brief Implements the server board. One can buy a card based on id, or trash a card based on id.
     */
    class ServerBoard : public shared::Board, public std::enable_shared_from_this<ServerBoard>
    {
    public:
        /**
         * @brief We use a shared_ptr instead of a unique_ptr because the reduced board and the server board have
         * exactly the same contents. This way we dont copy the contents each time we create a message.
         */
        using ptr_t = std::shared_ptr<ServerBoard>;
        using pile_container_t = shared::Board::pile_container_t;

        /**
         * @brief Constructs a ServerBoard for a given number of players and 10 kingdom cards.
         *
         * @param kingdom_cards 10 kingdom cards
         * @param player_count integer in range [2, 4]
         */
        static ptr_t make(const std::vector<shared::CardBase::id_t> &kingdom_cards, size_t player_count);

        /**
         * @brief Returns the reduced representation of the board (exactly the same as this one, but with less
         * functions)
         *
         * @return shared::Board::ptr_t
         */
        shared::Board::ptr_t getReduced();

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
        void trashCard(const shared::CardBase::id_t &card_id);

    protected:
        /**
         * @brief Construct a new Server Board object. This is protected to make testing easier and to enforce the use
         * of shared_ptr
         *
         * @param kingdom_cards
         * @param player_count
         */
        ServerBoard(const std::vector<shared::CardBase::id_t> &kingdom_cards, size_t player_count);
    };

} // namespace server
