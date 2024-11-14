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
        using ptr_t = std::shared_ptr<Board>;

        virtual ~Board() = default;

        // disable copy constructor and assignment
        Board(const Board &) = delete;
        Board &operator=(const Board &) = delete;

        // enable move semantics
        Board(Board &&) noexcept = default;
        Board &operator=(Board &&) noexcept = default;

        static ptr_t make(const std::vector<shared::CardBase::id_t> &kingdom_cards, size_t player_count)
        {
            return ptr_t(new Board(kingdom_cards, player_count));
        }

        bool isGameOver() const;
        size_t countEmptyPiles() const;

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
        Board(const std::vector<shared::CardBase::id_t> &kingdom_cards, size_t player_count)
        {
            _ASSERT_EQ(kingdom_cards.size(), 10, "Board must be initialised with 10 kingdom cards!");
            _ASSERT_TRUE((2 <= player_count && player_count <= 4), "Players must be in [2, 4]");

            this->kingdom_cards.reserve(kingdom_cards.size());
            std::transform(kingdom_cards.begin(), kingdom_cards.end(), std::back_inserter(this->kingdom_cards),
                           [](const shared::CardBase::id_t &card_id) {
                               return Pile{card_id, INITIAL_NUM_KINGDOM_CARDS};
                           });

            initialise_treasure_cards(player_count);
            initialise_victory_cards(player_count);
        }

        void initialise_treasure_cards(size_t player_count);
        void initialise_victory_cards(size_t player_count);
    };

} // namespace shared
