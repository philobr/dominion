#pragma once

#include <set>
#include <vector>

#include <shared/game/cards/card_base.h>
#include <shared/utils/assert.h>
namespace shared
{
    namespace BoardConfig
    {
        static constexpr size_t KINGDOM_CARD_COUNT = 10;

        static constexpr size_t TREASURE_COPPER_COUNT = 60;
        static constexpr size_t TREASURE_SILVER_COUNT = 40;
        static constexpr size_t TREASURE_GOLD_COUNT = 30;

        static constexpr size_t CURSE_MULTIPLIER = 10; // 10 curse cards per player
        static constexpr size_t VICTORY_CARDS_SMALL_GAME = 8; // for player count < 3
        static constexpr size_t VICTORY_CARDS_LARGE_GAME = 12; // for player count > 3

        static constexpr size_t MIN_PLAYER_COUNT = 2;
        static constexpr size_t MAX_PLAYER_COUNT = 4;

    } // namespace BoardConfig

    struct Pile
    {
        shared::CardBase::id_t card_id;
        mutable size_t count; // `mutable` allows modification const contexts

        static Pile makeKingdomCard(const shared::CardBase::id_t &kingdom_card_id)
        {
            return Pile(kingdom_card_id, BoardConfig::KINGDOM_CARD_COUNT);
        }

        static Pile make(const shared::CardBase::id_t &kingdom_card_id, size_t pile_size)
        {
            return Pile(kingdom_card_id, pile_size);
        }

        struct PileComparator
        {
            using is_transparent = void; // enables heterogeneous lookup

            bool operator()(const Pile &a, const Pile &b) const { return a.card_id < b.card_id; }
            bool operator()(const Pile &a, const shared::CardBase::id_t &key) const { return a.card_id < key; }
            bool operator()(const shared::CardBase::id_t &key, const Pile &b) const { return key < b.card_id; }
        };

    private:
        Pile(const shared::CardBase::id_t &new_card_id, size_t pile_size) : card_id(new_card_id), count(pile_size) {}
    };

    class Board
    {
    public:
        /**
         * @brief We use a shared_ptr instead of a unique_ptr because the reduced board and the server board
         * have exactly the same contents. This way we dont copy the contents each time we create a message.
         */
        using ptr_t = std::shared_ptr<Board>;
        using pile_container_t = std::set<Pile, Pile::PileComparator>;

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
        pile_container_t victory_cards;
        pile_container_t treasure_cards;
        pile_container_t kingdom_cards;
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
