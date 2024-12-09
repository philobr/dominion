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
    /**
     * Manages the server players state and provides useful functions.
     */
    class Player : public shared::PlayerBase
    {
        std::vector<shared::CardBase::id_t> draw_pile;
        std::vector<shared::CardBase::id_t> discard_pile;
        std::vector<shared::CardBase::id_t> hand_cards;

        std::vector<shared::CardBase::id_t> played_cards;
        std::vector<shared::CardBase::id_t> staged_cards;

    public:
        using id_t = shared::PlayerBase::id_t;
        using ptr_t = std::unique_ptr<Player>;
        using card_id = shared::CardBase::id_t;

        explicit Player(shared::PlayerBase::id_t id) : shared::PlayerBase(id){};

        Player(const Player &other) :
            shared::PlayerBase(other), draw_pile(other.draw_pile), discard_pile(other.discard_pile),
            hand_cards(other.hand_cards), played_cards(other.played_cards)
        {}

        reduced::Player::ptr_t getReducedPlayer();
        reduced::Enemy::ptr_t getReducedEnemy();

        void playAvailableTreasureCards();

        template <enum shared::CardAccess PILE>
        inline bool hasCard(const shared::CardBase::id_t &card_id) const;

        template <enum shared::CardAccess PILE>
        inline bool hasType(shared::CardType type) const;

        template <enum shared::CardAccess PILE>
        inline std::vector<shared::CardBase::id_t> getType(shared::CardType type) const;

        inline bool canBuy(unsigned int cost) { return buys > 0 && treasure >= cost; }
        inline bool canBlock() const { return hasType<shared::CardAccess::HAND>(shared::CardType::REACTION); }

        /**
         * @brief Draw the min(n, draw_pile_size) top cards from the deck and put them in the hand
         *
         * @param n number of cards to draw
         */
        inline void draw(size_t n) { move<shared::DRAW_PILE_TOP, shared::HAND>(n); }

        inline void playCardFromHand(const shared::CardBase::id_t &card_id)
        {
            move<shared::HAND, shared::PLAYED_CARDS>(card_id);
        }

        inline void playCardFromStaged(const shared::CardBase::id_t &card_id)
        {
            move<shared::STAGED_CARDS, shared::PLAYED_CARDS>(card_id);
        }

        /**
         * @brief Adds a card to the discard_pile
         */
        inline void gain(const shared::CardBase::id_t &card_id) { add<shared::DISCARD_PILE>(card_id); }

        /**
         * @brief Trashes the cards at the given indices
         */
        template <enum shared::CardAccess FROM_PILE>
        inline void trash(const std::vector<unsigned int> &indices);

        /**
         * @brief Moves the cards at the given pile to the back of the discard pile
         */
        template <enum shared::CardAccess FROM_PILE>
        inline void discard(const std::vector<unsigned int> &indices);

        /**
         * @brief Moves the cards at the given pile to the back of the stage pile
         */
        template <enum shared::CardAccess FROM_PILE>
        inline void stage(const std::vector<unsigned int> &indices);

        /**
         * @brief Unstages the cards at the specified indices to the specified pile.
         */
        template <enum shared::CardAccess TO_PILE>
        inline void unStage(const std::vector<unsigned int> &indices);

        void addActions(unsigned int n) { actions += n; }
        void addBuys(unsigned int n) { buys += n; }
        void addTreasure(unsigned int n) { treasure += n; }

        /**
         * @brief Moves the played_cards & hand_cards to the discard_pile, then draws 5 cards again.
         */
        void endTurn();

        template <enum shared::CardAccess PILE>
        inline const std::vector<shared::CardBase::id_t> &get() const;

        template <enum shared::CardAccess TO>
        inline void add(std::vector<shared::CardBase::id_t> &&cards);

        template <enum shared::CardAccess TO>
        inline void add(const std::vector<shared::CardBase::id_t> &cards);

        template <enum shared::CardAccess TO>
        inline void add(const shared::CardBase::id_t &card_id);

        template <enum shared::CardAccess FROM, enum shared::CardAccess TO>
        inline void move(const shared::CardBase::id_t &card_id);

        /**
         * @brief Get the victory points of the player.
         *
         * This includes the draw_pile, discard_pile and hand_cards.
         * This should only be called when played_cards and staged_cards are empty.
         */
        int getVictoryPoints() const;

    protected:
        /**
         * @brief Get all cards in the deck of the player.
         *
         * This includes the draw_pile, discard_pile and hand_cards.
         * This should only be called when played_cards and staged_cards are empty.
         */
        std::vector<shared::CardBase::id_t> getDeck() const;

        /**
         * @brief Resets the 'stats' to:
         * - actions            1
         * - buys               1
         * - treasure           0
         */
        void resetValues();

        template <enum shared::CardAccess PILE>
        inline std::vector<shared::CardBase::id_t> &getMutable();

        template <enum shared::CardAccess PILE>
        inline size_t getIndex(const shared::CardBase::id_t &card_id) const;

        template <enum shared::CardAccess PILE>
        inline void shuffle();


        /**
         * @brief Adds the given cards to the specified TO pile.
         * Will always perform a push_back, except for DRAW_PILE_BOTTOM
         * @throw std::invalid_argument
         * @tparam TO pile to which we add to
         */
        template <enum shared::CardAccess TO, typename Iterator>
        inline void add(Iterator begin, Iterator end);

        /**
         * @brief Removes $n$ cards from a pile.
         * Always takes the back, except for DRAW_PILE_TOP
         *
         * @tparam FROM, a pile from which we want to take cards
         * @param n
         * @return std::vector<shared::CardBase::id_t>
         */
        template <enum shared::CardAccess FROM>
        inline std::vector<shared::CardBase::id_t> take(unsigned int num_cards = 0);

        /**
         * @brief Removes the card at the index and returns it.
         * If the index is out of bounds we throw (gets checked before taking cards).
         *
         * @tparam FROM
         * @param index
         * @return shared::CardBase::id_t
         */
        template <enum shared::CardAccess FROM>
        inline shared::CardBase::id_t takeIndices(const size_t &index);

        /**
         * @brief Removes the cards at the indices and returns a vector of those.
         * If the indices are out of bounds we throw (gets checked before taking cards).
         *
         * @tparam FROM
         * @param indices
         * @return std::vector<shared::CardBase::id_t>
         */

        template <enum shared::CardAccess FROM>
        inline std::vector<shared::CardBase::id_t> takeIndices(const std::vector<unsigned int> &indices);

        /**
         * @brief Moves cards from pile FROM to pile TO (push_back, except for draw_pile top).
         * if FROM is TRASH we throw, if TO is TRASH we just delete the cards
         */
        template <enum shared::CardAccess FROM, enum shared::CardAccess TO>
        inline void move(unsigned int n = 0);

        /**
         * @brief Moves the card at the index form FROM to TO (push_back, except for draw_pile top).
         * We throw if index is out of bounds.
         * We throw if we try to move from trash.
         */
        template <enum shared::CardAccess FROM, enum shared::CardAccess TO>
        inline void moveIndices(const size_t &index);

        /**
         * @brief Moves the cards at the indices form FROM to TO (push_back, except for draw_pile top).
         * We throw if indices are out of bounds.
         * We throw if we try to move from trash.
         */
        template <enum shared::CardAccess FROM, enum shared::CardAccess TO>
        inline void moveIndices(const std::vector<unsigned int> &indices);
    };

#pragma region TO_BE_MOVED
    template <enum shared::CardAccess FROM>
    inline std::vector<shared::CardBase::id_t> Player::take(unsigned int n)
    {
        auto &pile = getMutable<FROM>();

        if ( n == 0 ) {
            n = pile.size();
        }

        if constexpr ( FROM == shared::DRAW_PILE_TOP || FROM == shared::DRAW_PILE_BOTTOM ) {
            // special case: we try to move discard pile to bottom of draw_pile
            if ( pile.size() < n && !getMutable<shared::DISCARD_PILE>().empty() ) {
                shuffle<shared::DISCARD_PILE>();
                move<shared::DISCARD_PILE, shared::DRAW_PILE_BOTTOM>();
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

        if constexpr ( FROM == shared::DRAW_PILE_TOP ) {
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

    template <enum shared::CardAccess FROM>
    inline shared::CardBase::id_t Player::takeIndices(const size_t &index)
    {
        auto &pile = getMutable<FROM>();

        // check for overflow
        if ( index >= pile.size() ) {
            LOG(ERROR) << "Index out of range in " << FUNC_NAME;
            throw std::out_of_range("Index out of range for the pile.");
        }

        shared::CardBase::id_t taken_card = std::move(pile[index]);
        pile.erase(pile.begin() + index);
        return taken_card;
    }

    template <enum shared::CardAccess FROM>
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


    template <enum shared::CardAccess FROM, enum shared::CardAccess TO>
    inline void Player::move(unsigned int n)
    {
        if constexpr ( TO == shared::TRASH ) {
            take<FROM>(n);
        } else {
            add<TO>(take<FROM>(n));
        }
    }

    template <enum shared::CardAccess FROM, enum shared::CardAccess TO>
    inline void Player::moveIndices(const size_t &index)
    {
        std::vector<unsigned int> indices = {static_cast<unsigned int>(index)};
        if constexpr ( TO == shared::TRASH ) {
            takeIndices<FROM>(indices);
        } else {
            add<TO>(takeIndices<FROM>(indices));
        }
    }

    template <enum shared::CardAccess FROM, enum shared::CardAccess TO>
    inline void Player::moveIndices(const std::vector<unsigned int> &indices)
    {
        if constexpr ( TO == shared::TRASH ) {
            takeIndices<FROM>(indices);
        } else {
            add<TO>(takeIndices<FROM>(indices));
        }
    }

#include "server_player.hpp"

} // namespace server
