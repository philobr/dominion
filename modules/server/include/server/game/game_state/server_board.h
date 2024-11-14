#pragma once

#include <vector>

#include <shared/game/game_state/board_base.h>
#include <shared/utils/assert.h>
namespace server
{
    // forward declarations
    class CardBase;

    /**
     * @brief Implements the server board. One can buy a card based on id, or trash a card based on id.
     */
    class ServerBoard : public shared::Board, public std::enable_shared_from_this<ServerBoard>
    {
    public:
        using ptr_t = std::shared_ptr<ServerBoard>;

        /**
         * @brief Constructs a ServerBoard for a given number of players and 10 kingdom cards.
         *
         * @param kingdom_cards 10 kingdom cards
         * @param player_count integer in range [2, 4]
         */
        static ptr_t make(const std::vector<shared::CardBase::id_t> &kingdom_cards, size_t player_count)
        {
            _ASSERT_TRUE((2 <= player_count && player_count <= 4), "Players must be in [2, 4]");
            return ptr_t(new ServerBoard(kingdom_cards, player_count));
        }

        shared::Board::ptr_t getReduced() { return std::static_pointer_cast<shared::Board>(shared_from_this()); }

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

    protected:
        ServerBoard(const std::vector<shared::CardBase::id_t> &kingdom_cards, size_t player_count);
    };

} // namespace server
