#pragma once

#include <vector>

#include <shared/game/cards/card_base.h>
#include <shared/utils/assert.h>
namespace shared
{
    struct Pile
    {
        shared::CardBase::id_t card;
        size_t count;
    };

    class Board
    {
    public:
        static constexpr size_t INITIAL_NUM_KINGDOM_CARDS = 10;

        /**
         * @brief We use a shared_ptr instead of a unique_ptr because the reduced board and the server board have
         * exactly the same contents. This way we dont copy the contents each time we create a message.
         */
        using ptr_t = std::shared_ptr<Board>;

        /**
         * @brief Constructs a shared_ptr on a ServerBoard for a given number of players and 10 kingdom cards.
         *
         * @param kingdom_cards 10 kingdom cards
         * @param player_count integer in range [2, 4]
         */
        static ptr_t make(const std::vector<shared::CardBase::id_t> &kingdom_cards, size_t player_count);

        virtual ~Board() = default;

        // enable move semantics
        Board(Board &&) noexcept = default;
        Board &operator=(Board &&) noexcept = default;

        // disable copy constructor and assignment
        Board(const Board &) = delete;
        Board &operator=(const Board &) = delete;

        /**
         * @brief Checks if provinces or at least 3 piles are emptied.
         *
         * @return true
         * @return false
         */
        bool isGameOver() const;

        /**
         * @brief Counts how many piles are empty overall.
         *
         * @return size_t
         */
        size_t getEmptyPilesCount() const;

    protected:
        std::vector<Pile> victory_cards;
        std::vector<Pile> treasure_cards;
        std::vector<Pile> kingdom_cards;
        std::vector<shared::CardBase::id_t> trash;

        /**
         * @brief Constructs a new board. It is assumed that the type is valid, as we have no way of checking it.
         * ServerBoard validates card types.
         *
         * @param kingdom_cards 10 valid kingdom cards
         * @param player_count number of players in [2, 4]
         */
        Board(const std::vector<shared::CardBase::id_t> &kingdom_cards, size_t player_count);

        /**
         * @brief Initialised the treasure cards as follows:
         * copper_count     60 - (7 * player_count)
         * silver_count     40
         * gold_count       30
         *
         * @param player_count
         */
        void initialiseTreasureCards(size_t player_count);

        /**
         * @brief Initialised the victory cards as follows:
         * card_count   8   player_count < 3
         *              12  player_count >= 3
         * curse_count  (player_count - 1) * 10;
         *
         * @param player_count
         */
        void initialiseVictoryCards(size_t player_count);
    };

} // namespace shared
