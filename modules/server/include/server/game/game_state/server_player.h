#pragma once

#include <deque>
#include <vector>

#include <shared/game/cards/card_base.h>
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
    public:
        using id_t = shared::PlayerBase::id_t;
        using ptr_t = std::unique_ptr<Player>;
        using card_id = shared::CardBase::id_t;

        /**
         * @brief Creates a new player based on an id.
         * @param id the new players name
         */
        explicit Player(shared::PlayerBase::id_t id) : shared::PlayerBase(id){};

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
         * @brief Peek the min(n, draw_pile_size) top cards from the deck.
         * The draw pile does NOT get manipulated through this.
         *
         * @param n number of cards to peek
         * @return std::vector<card_id> [ deck_top_card == vector[i], deck_(n-1)th_card = vector[n - 1]
         */
        std::vector<card_id> peek_draw_pile(size_t n);

        /**
         * @brief Draw the min(n, draw_pile_size) top cards from the deck and put them in the hand
         *
         * @param n number of cards to draw
         */
        void draw(size_t n);

        /**
         * @brief Deletes the card stored in range at index.
         *
         * @param range a valid range of card_ids
         * @param trash_idx index in [0, range.size())
         */
        void trash(std::vector<card_id> &range, size_t trash_idx);

        /**
         * @brief Moves the card stored in range at index to the top of the discard_pile
         *
         * @param range a valid range of card_ids
         * @param trash_idx index in [0, range.size())
         */
        void discard(std::vector<card_id> &range, size_t trash_idx);

        /**
         * @brief Adds to the range. (push_back)
         *
         * @param range
         * @param trash_idx a (not validated) card id
         */
        // TODO: this needs to be add_top, add_bottom
        void add(std::vector<card_id> &range, const card_id &card) { range.push_back(card); }

        /**
         * @brief Adds a card to the discard_pile. (push_back)
         *
         * @param card
         */
        void add_to_discard_pile(const card_id &card) { discard_pile.push_back(card); }

        /**
         * @brief Moves the played_cards & hand_cards to the discard_pile, then draws 5 cards again.
         */
        void end_turn();

        // TODO: there is technically no reason to expose those functions, refactor them away (if possible/usefull)
        auto &get_draw_pile() { return draw_pile; }
        auto &get_discard_pile() { return discard_pile; }
        auto &get_hand_cards() { return hand_cards; }
        auto &get_current_card() { return currently_playing_card; }
        auto &get_played_cards() { return played_cards; }

        void incActions(int n) { actions += n; }
        void incBuys(int n) { buys += n; }
        void incTreasure(int n) { treasure += n; }
        void incPoints(int n) { victory_points += n; }

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
         * @brief Shuffles all cards in the discard_pile and inserts them below the draw_pile.
         */
        void shuffle_deck();

        enum CardAccess
        {
            DISCARD_PILE = 1,
            DRAW_PILE_PILE_TOP = 2,
            DRAW_PILE_PILE_BOTTOM = 4,
            HAND = 8,

            PLAYED_CARDS = 16,
            STAGED_CARDS = 32, // ex: sentry could move here
            TRASH = 64,
        };

        std::vector<shared::CardBase::id_t> &get_pile(CardAccess pile)
        {
            switch ( pile ) {
                case DISCARD_PILE:
                    return discard_pile;
                case HAND:
                    return hand_cards;
                case PLAYED_CARDS:
                    return played_cards;
                default:
                    throw std::invalid_argument("Invalid pile specified or pile is not accessible.");
            }
        }

        std::vector<shared::CardBase::id_t> take_n_from(unsigned int n, CardAccess from)
        {
            std::vector<shared::CardBase::id_t> taken_cards;
            taken_cards.reserve(n);

            switch ( from ) {
                case DISCARD_PILE:
                case HAND:
                case PLAYED_CARDS:
                    {
                        auto &pile = get_pile(from);
                        n = std::min(n, static_cast<unsigned int>(pile.size()));
                        auto start_iter = pile.end() - n;
                        taken_cards = {std::make_move_iterator(pile.end() - n), std::make_move_iterator(pile.end())};
                        pile.erase(start_iter, pile.end());
                    }
                    break;
                case DRAW_PILE_PILE_TOP:
                    {
                        auto &pile = draw_pile;
                        n = std::min(n, static_cast<unsigned int>(pile.size()));
                        taken_cards = {std::make_move_iterator(draw_pile.begin()),
                                       std::make_move_iterator(draw_pile.begin() + n)};
                        pile.erase(pile.begin(), pile.begin() + n);
                    }
                    break;
                case DRAW_PILE_PILE_BOTTOM:
                    {
                        auto &pile = draw_pile;
                        n = std::min(n, static_cast<unsigned int>(pile.size()));
                        taken_cards = {std::make_move_iterator(draw_pile.end() - n),
                                       std::make_move_iterator(draw_pile.end())};
                        pile.erase(pile.end() - n, pile.end());
                    }
                    break;
                default:
                    throw std::invalid_argument("Invalid 'from' pile specified.");
            }

            return taken_cards;
        }

        void add_to(std::vector<shared::CardBase::id_t> cards, CardAccess to)
        {
            switch ( to ) {
                case DISCARD_PILE:
                case HAND:
                case PLAYED_CARDS:
                    {
                        auto &pile = get_pile(to);
                        pile.insert(discard_pile.end(), std::make_move_iterator(cards.begin()),
                                    std::make_move_iterator(cards.end()));
                    }
                    break;
                case DRAW_PILE_PILE_TOP:
                    {
                        draw_pile.insert(draw_pile.begin(), std::make_move_iterator(cards.begin()),
                                         std::make_move_iterator(cards.end()));
                    }
                    break;
                case DRAW_PILE_PILE_BOTTOM:
                    {
                        draw_pile.insert(draw_pile.end(), std::make_move_iterator(cards.begin()),
                                         std::make_move_iterator(cards.end()));
                    }
                    break;
                case TRASH:
                    {
                        // do nothing, cards are already trashed implicitly
                    }
                    break;
                default:
                    throw std::invalid_argument("Invalid 'to' pile specified.");
            }
        }

        void move(int n, CardAccess from, CardAccess to)
        {
            if ( n <= 0 )
                return;

            if ( to == TRASH ) {
                take_n_from(n, from);
            } else {
                auto cards = take_n_from(n, from);
                add_to(std::move(cards), to);
            }
        }

        /**
         * we can expose stuff like:
         * - getNFromImmutable
         * - moveToTrash(from)
         *
         * actually, this can even be templated!!!!!
         */


        std::deque<card_id> draw_pile;
        std::vector<card_id> discard_pile;
        std::vector<card_id> hand_cards;
        std::vector<card_id> played_cards; // TODO: move this to base player

        // TODO: this is ugly, rename or smth
        card_id currently_playing_card;
        size_t current_behaviour_idx;
    };
} // namespace server