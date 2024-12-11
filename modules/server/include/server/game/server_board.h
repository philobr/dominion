#pragma once

#include <vector>

#include <shared/game/game_state/board_base.h>
#include <shared/utils/assert.h>
#include <shared/utils/logger.h>

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
         */
        shared::Board::ptr_t getReduced();

        /**
         * @brief Throws if the card_id one wants to buy is not available.
         */
        void tryTake(const shared::CardBase::id_t &card_id);

        /**
         * @brief Checks if the card exists on the board.
         */
        bool has(const shared::CardBase::id_t &card_id) const;

        /**
         * @brief Adds the given card to the played_cards vector
         */
        void addToPlayedCards(const shared::CardBase::id_t &card_id);

        /**
         * @brief Removes the given card from the played_cards vector
         *
         * @return true if the card was removed
         * @return false if the card was not found
         */
        bool removeFromPlayedCards(const shared::CardBase::id_t &card_id);

        /**
         * @brief Adds the card_id to the trash
         */
        void trashCard(const shared::CardBase::id_t &card_id);

        void resetPlayedCards() { played_cards.clear(); }

    protected:
        /**
         * @brief Construct a new Server Board object. This is protected to make testing easier and to enforce the use
         */
        ServerBoard(const std::vector<shared::CardBase::id_t> &kingdom_cards, size_t player_count);

        /**
         * @brief Tries to buy a card based on id.
         *
         * @param card
         * @return true successfully bought the card
         * @return false card_id can not be bought
         */
        void take(const shared::CardBase::id_t &card_id);
    };

} // namespace server
