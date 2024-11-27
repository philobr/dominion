#pragma once

#include <deque>
#include <random>
#include <vector>

#include <shared/game/cards/card_base.h>
#include <shared/game/cards/card_factory.h>
#include <shared/game/game_state/player_base.h>
#include <shared/game/game_state/reduced_game_state.h>

#include <shared/utils/logger.h>

namespace server
{
    // TODO: move this inside the playerbase class? (does this help gui people?)
    enum CardAccess
    {
        DISCARD_PILE = 1,
        HAND = 2,
        DRAW_PILE_TOP = 4,
        DRAW_PILE_BOTTOM = 8,

        PLAYED_CARDS = 16,
        TRASH = 32,
        STAGED_CARDS = 64 // ex: sentry could move here
    };

    /**
     * Manages the server players state and provides useful functions.
     */
    class Player : public shared::PlayerBase
    {
        std::vector<shared::CardBase::id_t> draw_pile;
        std::vector<shared::CardBase::id_t> discard_pile;
        std::vector<shared::CardBase::id_t> hand_cards;

        // TODO: move this inside the playerbase class? (does this help gui people?)
        std::vector<shared::CardBase::id_t> played_cards; // TODO: move this to base player
        std::vector<shared::CardBase::id_t> staged_cards; // TODO: move this to base player

        // TODO: this is ugly, rename or smth
        shared::CardBase::id_t currently_playing_card;
        size_t current_behaviour_idx;

    public:
        using id_t = shared::PlayerBase::id_t;
        using ptr_t = std::unique_ptr<Player>;
        using card_id = shared::CardBase::id_t;

        /**
         * @brief Creates a new player based on an id.
         * @param id the new players name
         */
        explicit Player(shared::PlayerBase::id_t id) : shared::PlayerBase(id) {};

        /**
         * @brief Copy constructor
         * @param other
         */
        Player(const Player &other) :
            shared::PlayerBase(other), draw_pile(other.draw_pile), discard_pile(other.discard_pile),
            hand_cards(other.hand_cards), played_cards(other.played_cards),
            currently_playing_card(other.currently_playing_card), current_behaviour_idx(other.current_behaviour_idx)
        {}

        reduced::Player::ptr_t getReducedPlayer();
        reduced::Enemy::ptr_t getReducedEnemy();

        bool canBlock() const
        {
            return std::any_of(hand_cards.begin(), hand_cards.end(),
                               [&](const auto &card_id) { return shared::CardFactory::getCard(card_id).isReaction(); });
        }

        /**
         * @brief Draw the min(n, draw_pile_size) top cards from the deck and put them in the hand
         *
         * @param n number of cards to draw
         */
        inline void draw(size_t n) { move<DRAW_PILE_TOP, HAND>(n); }

        /**
         * @brief Adds a card to the discard_pile
         */
        inline void gain(const shared::CardBase::id_t &card_id) { add<DISCARD_PILE>(card_id); }

        /**
         * @brief Trashes the cards at the given indices
         *
         * @tparam FROM_PILE
         * @param indices
         */
        template <enum CardAccess FROM_PILE>
        inline void trash(const std::vector<unsigned int> &indices);

        /**
         * @brief Moves the cards at the given pile to the back of the discard pile
         *
         * @tparam FROM_PILE
         * @param indices
         */
        template <enum CardAccess FROM_PILE>
        inline void discard(const std::vector<unsigned int> &indices);

        /**
         * @brief Moves the cards at the given pile to the back of the stage pile
         *
         * @tparam FROM_PILE
         * @param indices
         */
        template <enum CardAccess FROM_PILE>
        inline void stage(const std::vector<unsigned int> &indices);

        /**
         * @brief Unstages the cards at the specified indices to the specified pile.
         *
         * @tparam TO_PILE
         * @param indices
         */
        template <enum CardAccess TO_PILE>
        inline void unStage(const std::vector<unsigned int> &indices);

        void addActions(unsigned int n) { actions += n; }
        void addBuys(unsigned int n) { buys += n; }
        void addTreasure(unsigned int n) { treasure += n; }
        void addPoints(unsigned int n) { victory_points += n; }

        /**
         * @brief Returns a const ref to the specified pile
         *
         * @tparam PILE
         * @return const std::vector<shared::CardBase::id_t>&
         */
        template <enum CardAccess PILE>
        inline const std::vector<shared::CardBase::id_t> &get();

        /**
         * @brief Moves the played_cards & hand_cards to the discard_pile, then draws 5 cards again.
         */
        void endTurn();

    protected:
        /**
         * @brief Resets the 'stats' to:
         * - actions                1
         * - buys                   1
         * - treasure               0
         * - victory_points         0
         */
        void resetValues();

        template <enum CardAccess PILE>
        inline std::vector<shared::CardBase::id_t> &getMutable();

        template <enum CardAccess PILE>
        inline void shuffle();

        /**
         * @brief Adds (no copy) the vector of cards to the specified pile.
         *
         * @tparam TO
         * @param cards
         */
        template <enum CardAccess TO>
        inline void add(std::vector<shared::CardBase::id_t> &&cards);

        /**
         * @brief Adds (with copy) the vector of cards to the specified pile.
         *
         * @tparam TO
         * @param cards
         */
        template <enum CardAccess TO>
        inline void add(const std::vector<shared::CardBase::id_t> &cards);

        /**
         * @brief Adds the card_id to the back of the specified pile
         *
         * @tparam TO
         * @param card_id
         */
        template <enum CardAccess TO>
        inline void add(const shared::CardBase::id_t &card_id);

        /**
         * @brief Removes $n$ cards from a pile.
         * Always takes the back, except for DRAW_PILE_TOP
         *
         * @tparam FROM, a pile from which we want to take cards
         * @param n
         * @return std::vector<shared::CardBase::id_t>
         */
        template <enum CardAccess FROM>
        inline std::vector<shared::CardBase::id_t> take(unsigned int num_cards = 0);

        /**
         * @brief Removes the cards at the indices and returns a vector of those.
         * If the indices are out of bounds we throw (gets checked before taking cards).
         *
         * @tparam FROM
         * @param indices
         * @return std::vector<shared::CardBase::id_t>
         */
        template <enum CardAccess FROM>
        inline std::vector<shared::CardBase::id_t> takeIndices(const std::vector<unsigned int> &indices);

        /**
         * @brief Moves cards from pile FROM to pile TO (push_back, except for draw_pile top).
         * if FROM is TRASH we throw, if TO is TRASH we just delete the cards
         *
         * @tparam FROM
         * @tparam TO
         * @param n number of cards to move
         */
        template <enum CardAccess FROM, enum CardAccess TO>
        inline void move(unsigned int n = 0);

        /**
         * @brief Moves the cards at the indices form FROM to TO (push_back, except for draw_pile top).
         * We throw if indices are out of bounds.
         * We throw if we try to move from trash.
         *
         * @tparam FROM
         * @tparam TO
         * @param indices
         */
        template <enum CardAccess FROM, enum CardAccess TO>
        inline void moveIndices(const std::vector<unsigned int> &indices);
    };

    template <enum CardAccess PILE>
    inline std::vector<shared::CardBase::id_t> &Player::getMutable()
    {
        if constexpr ( PILE == DISCARD_PILE ) {
            return discard_pile;
        } else if constexpr ( PILE == HAND ) {
            return hand_cards;
        } else if constexpr ( PILE == PLAYED_CARDS ) {
            return played_cards;
        } else if constexpr ( PILE == STAGED_CARDS ) {
            return staged_cards;
        } else if constexpr ( PILE == DRAW_PILE_TOP ) {
            return draw_pile;
        } else if constexpr ( PILE == DRAW_PILE_BOTTOM ) {
            return draw_pile;
        } else {
            // should only happen for trash pile
            throw std::invalid_argument("Invalid pile specified or pile is not accessible.");
        }
    }

    template <enum CardAccess PILE>
    inline void Player::shuffle()
    {
        static std::random_device rd; // we only need one instance
        static std::mt19937 gen(rd()); // we only need one instance

        auto &cards = getMutable<PILE>();
        std::shuffle(cards.begin(), cards.end(), gen);
    }

    template <enum CardAccess FROM>
    inline std::vector<shared::CardBase::id_t> Player::take(unsigned int n)
    {
        auto &pile = getMutable<FROM>();

        if ( n == 0 ) {
            n = pile.size();
        }

        if constexpr ( FROM == DRAW_PILE_TOP || FROM == DRAW_PILE_BOTTOM ) {
            // special case: we try to move discard pile to bottom of draw_pile
            if ( pile.size() < n && !getMutable<DISCARD_PILE>().empty() ) {
                shuffle<DISCARD_PILE>();
                move<DISCARD_PILE, DRAW_PILE_BOTTOM>();
            }
            n = std::min(n, static_cast<unsigned int>(pile.size()));
        } else {
            // throw if we dont have enough cards
            if ( pile.size() < n ) {
                throw std::runtime_error("Not enough cards in the pile to take.");
            }
        }

        std::vector<shared::CardBase::id_t> taken_cards;
        taken_cards.reserve(n);

        if constexpr ( FROM == DRAW_PILE_TOP ) {
            // take from top
            taken_cards.assign(std::make_move_iterator(pile.begin()), std::make_move_iterator(pile.begin() + n));
            pile.erase(pile.begin(), pile.begin() + n);
        } else {
            // take from back
            taken_cards.assign(std::make_move_iterator(pile.end() - n), std::make_move_iterator(pile.end()));
            pile.erase(pile.end() - n, pile.end());
        }

        return taken_cards;
    }

    template <enum CardAccess FROM>
    inline std::vector<shared::CardBase::id_t> Player::takeIndices(const std::vector<unsigned int> &indices)
    {
        auto &pile = getMutable<FROM>();

        // sort desc order to safely erase
        std::vector<unsigned int> sorted_indices = indices;
        std::sort(sorted_indices.rbegin(), sorted_indices.rend());

        // check for overflow
        if ( sorted_indices.front() >= pile.size() ) {
            throw std::out_of_range("Index out of range for the pile.");
        }

        std::vector<shared::CardBase::id_t> taken_cards;
        taken_cards.reserve(indices.size());

        for ( unsigned int idx : sorted_indices ) {
            taken_cards.push_back(std::move(pile[idx]));
            pile.erase(pile.begin() + idx);
        }

        std::reverse(taken_cards.begin(), taken_cards.end()); // maintain original order
        return taken_cards;
    }

    template <enum CardAccess TO>
    inline void Player::add(std::vector<shared::CardBase::id_t> &&cards)
    {
        auto &pile = getMutable<TO>();
        if constexpr ( TO == DRAW_PILE_TOP ) {
            pile.insert(pile.begin(), std::make_move_iterator(cards.begin()), std::make_move_iterator(cards.end()));
        } else {
            pile.insert(pile.end(), std::make_move_iterator(cards.begin()), std::make_move_iterator(cards.end()));
        }
    }

    template <enum CardAccess TO>
    inline void Player::add(const shared::CardBase::id_t &card_id)
    {
        auto &pile = getMutable<TO>();
        if constexpr ( TO == DRAW_PILE_TOP ) {
            pile.push_front(card_id);
        } else {
            pile.push_back(card_id);
        }
    }

    template <enum CardAccess TO>
    inline void Player::add(const std::vector<shared::CardBase::id_t> &cards)
    {
        auto &pile = getMutable<TO>();
        if constexpr ( TO == DRAW_PILE_TOP ) {
            pile.insert(pile.begin(), cards.begin(), cards.end());
        } else {
            pile.insert(pile.end(), cards.begin(), cards.end());
        }
    }

    template <enum CardAccess FROM, enum CardAccess TO>
    inline void Player::move(unsigned int n)
    {
        if constexpr ( TO == TRASH ) {
            take<FROM>(n);
        } else {
            add<TO>(take<FROM>(n));
        }
    }

    template <enum CardAccess FROM, enum CardAccess TO>
    inline void Player::moveIndices(const std::vector<unsigned int> &indices)
    {
        if constexpr ( TO == TRASH ) {
            takeIndices<FROM>(indices);
        } else {
            add<TO>(takeIndices<FROM>(indices));
        }
    }

    template <enum CardAccess PILE>
    inline void Player::trash(const std::vector<unsigned int> &indices)
    {
        moveIndices<PILE, TRASH>(indices);
    }

    template <enum CardAccess PILE>
    inline void Player::discard(const std::vector<unsigned int> &indices)
    {
        moveIndices<PILE, DISCARD_PILE>(indices);
    }

    template <enum CardAccess PILE>
    inline const std::vector<shared::CardBase::id_t> &Player::get()
    {
        return getMutable<PILE>();
    }

    template <enum CardAccess PILE>
    inline void Player::stage(const std::vector<unsigned int> &indices)
    {
        move_indices<PILE, STAGED_CARDS>(indices);
    }

    template <enum CardAccess TO_PILE>
    inline void Player::unStage(const std::vector<unsigned int> &indices)
    {
        move_indices<STAGED_CARDS, TO_PILE>(indices);
    }
} // namespace server
