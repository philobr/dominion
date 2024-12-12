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
        std::vector<shared::CardBase::id_t> hand_cards;

        std::vector<shared::CardBase::id_t> staged_cards;

    public:
        using id_t = shared::PlayerBase::id_t;
        using ptr_t = std::unique_ptr<Player>;
        using card_id = shared::CardBase::id_t;

        explicit Player(shared::PlayerBase::id_t id) : shared::PlayerBase(id){};

        Player(const Player &other) :
            shared::PlayerBase(other), draw_pile(other.draw_pile), hand_cards(other.hand_cards)
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

        /**
         * @brief Adds a card to the discard_pile
         */
        inline void gain(const shared::CardBase::id_t &card_id) { add<shared::DISCARD_PILE>(card_id); }

        void addActions(unsigned int n) { actions += n; }
        void addBuys(unsigned int n) { buys += n; }
        void addTreasure(unsigned int n) { treasure += n; }

        /**
         * @brief Moves the played_cards & hand_cards to the discard_pile, then draws 5 cards again.
         */
        void endTurn();

        /**
         * @brief Get the victory points of the player.
         *
         * This includes the draw_pile, discard_pile and hand_cards.
         * This should only be called when played_cards and staged_cards are empty.
         */
        int getVictoryPoints() const;

        /**
         * @return A const reference to the indicated pile.
         * @warning Throws if we try to access the trash pile.
         */
        template <enum shared::CardAccess PILE>
        inline const std::vector<shared::CardBase::id_t> &get() const;

        /**
         * @brief Adds a card to the specified pile.
         */
        template <enum shared::CardAccess TO>
        inline void add(std::vector<shared::CardBase::id_t> &&cards);

        /**
         * @brief Adds a card to the specified pile.
         */
        template <enum shared::CardAccess TO>
        inline void add(const std::vector<shared::CardBase::id_t> &cards);

        /**
         * @brief Adds a card to the specified pile.
         */
        template <enum shared::CardAccess TO>
        inline void add(const shared::CardBase::id_t &card_id);

        /**
         * @brief Moves the card ID from pile FROM to pile TO. Trashed cards are simply deleted.
         */
        template <enum shared::CardAccess FROM, enum shared::CardAccess TO>
        inline void move(const shared::CardBase::id_t &card_id);

        /**
         * @brief Moves the card IDs from pile FROM to pile TO. Trashed cards are simply deleted.
         */
        template <enum shared::CardAccess FROM, enum shared::CardAccess TO>
        inline void move(const std::vector<shared::CardBase::id_t> &cards);

        /**
         * @brief Moves the first min(n, pile.size()) cards from pile FROM to pile TO.
         */
        template <enum shared::CardAccess FROM, enum shared::CardAccess TO>
        inline void move(unsigned int n = 0);

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

        /**
         * @return A mutable reference to the indicated pile.
         * @warning Throws if one tries to access the trash pile.
         */
        template <enum shared::CardAccess PILE>
        inline std::vector<shared::CardBase::id_t> &getMutable();

        /**
         * @brief Shuffles the indicated pile PILE
         */
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
         * @brief Removes a card with ID 'card_id' from the indicated pile.
         * @return The same ID we passed in.
         * @warning Throws
         */
        template <enum shared::CardAccess FROM>
        inline shared::CardBase::id_t take(const shared::CardBase::id_t &card_id);

        /**
         * @brief Removes the card_ids 'cards' with card_id from the indicated pile.
         * @return The same vector we passed in.
         * @warning Throws
         */
        template <enum shared::CardAccess FROM>
        inline std::vector<shared::CardBase::id_t> take(const std::vector<shared::CardBase::id_t> &cards);

        /**
         * @brief Removes min(n, pile.size()) cards from a pile.
         * Always takes the back, except for DRAW_PILE_TOP
         *
         * @tparam FROM, a pile from which we want to take cards
         * @param n
         * @return std::vector<shared::CardBase::id_t>
         */
        template <enum shared::CardAccess FROM>
        inline std::vector<shared::CardBase::id_t> take(unsigned int num_cards = 0);
    };

#include "server_player.hpp"

} // namespace server
