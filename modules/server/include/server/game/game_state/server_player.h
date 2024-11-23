#pragma once

#include <deque>
#include <random>
#include <vector>

#include <shared/game/cards/card_base.h>
#include <shared/game/game_state/player_base.h>
#include <shared/game/game_state/reduced_game_state.h>
#include <shared/utils/logger.h>

namespace server
{
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

        // TODO: maybe add some operators?
        // ~Player();
        // Player(Player &&other);
        // Player &operator=(const Player &other);
        // bool operator==(const Player &other);
        // friend std::ostream &operator<<(std::ostream &os, const Player &player); // is this overkill (logging, etc)

        shared::ReducedPlayer::ptr_t get_reduced_player();
        shared::ReducedEnemy::ptr_t get_reduced_enemy();

        /**
         * @brief Draw the min(n, draw_pile_size) top cards from the deck and put them in the hand
         *
         * @param n number of cards to draw
         */
        inline void draw(size_t n) { move<DRAW_PILE_TOP, HAND>(n); }

        template <enum CardAccess PILE>
        inline void trash(const std::vector<unsigned int> &indices)
        {
            move_indices<PILE, TRASH>(indices);
        }

        template <enum CardAccess PILE>
        inline void discard(const std::vector<unsigned int> &indices)
        {
            move_indices<PILE, DISCARD_PILE>(indices);
        }

        void peek(int n) { move<DRAW_PILE_TOP, STAGED_CARDS>(n); }
        void unpeek() { move<STAGED_CARDS, DRAW_PILE_TOP>(); }

        /**
         * @brief Moves the played_cards & hand_cards to the discard_pile, then draws 5 cards again.
         */
        void end_turn();

        void incActions(int n) { actions += n; }
        void incBuys(int n) { buys += n; }
        void incTreasure(int n) { treasure += n; }
        void incPoints(int n) { victory_points += n; }

        template <enum CardAccess PILE>
        inline std::vector<shared::CardBase::id_t> &get();

        template <enum CardAccess PILE>
        inline void shuffle();

        template <enum CardAccess FROM>
        inline std::vector<shared::CardBase::id_t> take(unsigned int n = 0);

        template <enum CardAccess FROM>
        inline std::vector<shared::CardBase::id_t> take_indices(const std::vector<unsigned int> &indices);

        template <enum CardAccess TO>
        inline void add(std::vector<shared::CardBase::id_t> &&cards);

        template <enum CardAccess TO>
        inline void add(const std::vector<shared::CardBase::id_t> &cards);

        template <enum CardAccess TO>
        inline void add(const shared::CardBase::id_t &card_id);

        template <enum CardAccess FROM, enum CardAccess TO>
        inline void move(unsigned int n = 0);

        template <enum CardAccess FROM, enum CardAccess TO>
        inline void move_indices(const std::vector<unsigned int> &indices);

    private:
        /**
         * @brief Resets the 'stats' to:
         * - actions      1
         * - buys         1
         * - treasure     0
         * - victory_points         0
         */
        void reset_values();

        /**
         * we can expose stuff like:
         * - getNFromImmutable
         * - moveToTrash(from)
         *
         * actually, this can even be templated!!!!!
         */


        std::vector<card_id> draw_pile;
        std::vector<card_id> discard_pile;
        std::vector<card_id> hand_cards;
        std::vector<card_id> played_cards; // TODO: move this to base player
        std::vector<card_id> staged_cards; // TODO: move this to base player

        // TODO: this is ugly, rename or smth
        card_id currently_playing_card;
        size_t current_behaviour_idx;
    };

    template <enum CardAccess PILE>
    inline std::vector<shared::CardBase::id_t> &Player::get()
    {
        switch ( PILE ) {
            case DISCARD_PILE:
                return discard_pile;
            case HAND:
                return hand_cards;
            case PLAYED_CARDS:
                return played_cards;
            case STAGED_CARDS:
                return staged_cards;
            case DRAW_PILE_TOP:
                return draw_pile;
            case DRAW_PILE_BOTTOM:
                return draw_pile;
            default:
                throw std::invalid_argument("Invalid pile specified or pile is not accessible.");
        }
    }

    template <enum CardAccess PILE>
    inline void Player::shuffle()
    {
        static std::random_device rd; // we only need one instance
        static std::mt19937 gen(rd()); // we only need one instance

        auto &cards = get<PILE>();
        std::shuffle(cards.begin(), cards.end(), gen); // Shuffle the cards
    }

    template <enum CardAccess FROM>
    inline std::vector<shared::CardBase::id_t> Player::take(unsigned int n)
    {
        auto &pile = get<FROM>();

        if ( n == 0 ) {
            n = pile.size();
        }

        // Special handling for draw pile: reshuffle if needed
        if constexpr ( FROM == DRAW_PILE_TOP || FROM == DRAW_PILE_BOTTOM ) {
            if ( pile.size() < n && !get<DISCARD_PILE>().empty() ) {
                shuffle<DISCARD_PILE>();
                move<DISCARD_PILE, DRAW_PILE_BOTTOM>();
            }
            n = std::min(n, static_cast<unsigned int>(pile.size()));
        } else {
            // For other piles, throw if not enough cards
            if ( pile.size() < n ) {
                throw std::runtime_error("Not enough cards in the pile to take.");
            }
        }

        // Take cards
        std::vector<shared::CardBase::id_t> taken_cards;
        taken_cards.reserve(n);

        if constexpr ( FROM == DRAW_PILE_TOP ) {
            // Take from the top of the draw pile
            taken_cards.assign(std::make_move_iterator(pile.begin()), std::make_move_iterator(pile.begin() + n));
            pile.erase(pile.begin(), pile.begin() + n);
        } else {
            // Take from the bottom or other piles
            taken_cards.assign(std::make_move_iterator(pile.end() - n), std::make_move_iterator(pile.end()));
            pile.erase(pile.end() - n, pile.end());
        }

        return taken_cards;
    }

    template <enum CardAccess FROM>
    inline std::vector<shared::CardBase::id_t> Player::take_indices(const std::vector<unsigned int> &indices)
    {
        auto &pile = get<FROM>();

        // Validate indices
        for ( unsigned int idx : indices ) {
            if ( idx >= pile.size() ) {
                throw std::out_of_range("Index out of range for the pile.");
            }
        }

        // Extract cards
        std::vector<shared::CardBase::id_t> taken_cards;
        taken_cards.reserve(indices.size());

        // Sort indices in descending order to safely erase from the pile
        std::vector<unsigned int> sorted_indices = indices;
        std::sort(sorted_indices.rbegin(), sorted_indices.rend());

        for ( unsigned int idx : sorted_indices ) {
            taken_cards.push_back(std::move(pile[idx]));
            pile.erase(pile.begin() + idx);
        }

        // Reverse the order of the taken cards to maintain the original order
        std::reverse(taken_cards.begin(), taken_cards.end());
        return taken_cards;
    }

    template <enum CardAccess TO>
    inline void Player::add(std::vector<shared::CardBase::id_t> &&cards)
    {
        auto &pile = get<TO>();
        if constexpr ( TO == DRAW_PILE_TOP ) {
            pile.insert(pile.begin(), std::make_move_iterator(cards.begin()), std::make_move_iterator(cards.end()));
        } else {
            pile.insert(pile.end(), std::make_move_iterator(cards.begin()), std::make_move_iterator(cards.end()));
        }
    }

    template <enum CardAccess TO>
    inline void Player::add(const shared::CardBase::id_t &card_id)
    {
        auto &pile = get<TO>();
        if constexpr ( TO == DRAW_PILE_TOP ) {
            pile.push_front(card_id);
        } else {
            pile.push_back(card_id);
        }
    }

    template <enum CardAccess TO>
    inline void Player::add(const std::vector<shared::CardBase::id_t> &cards)
    {
        auto &pile = get<TO>();
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
    inline void Player::move_indices(const std::vector<unsigned int> &indices)
    {
        if constexpr ( TO == TRASH ) {
            take_indices<FROM>(indices);
        } else {
            add<TO>(take_indices<FROM>(indices));
        }
    }
} // namespace server
