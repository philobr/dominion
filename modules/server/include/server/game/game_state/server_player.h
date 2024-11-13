#pragma once

#include <deque>
#include <vector>

#include <shared/game/cards/card_base.h>
#include <shared/game/game_state/player_base.h>
#include <shared/game/game_state/reduced_game_state.h>

namespace server
{
    /**
     * @brief Player represents a player on the server (obviously ;) ).
     *
     * This class holds all the information about a player that is needed on the server.
     */
    class Player : public shared::PlayerBase
    {
    public:
        using id_t = shared::PlayerBase::id_t;
        using ptr_t = std::unique_ptr<Player>;
        using card_id = shared::CardBase::id_t;

        Player(){};
        Player(shared::PlayerBase::id_t id) : shared::PlayerBase(id){};
        Player(const Player &other) :
            shared::PlayerBase(other), // Copy base class data
            draw_pile(other.draw_pile), discard_pile(other.discard_pile), hand_cards(other.hand_cards),
            played_cards(other.played_cards), currently_playing_card(other.currently_playing_card),
            current_behaviour_idx(other.current_behaviour_idx)
        {}

        std::vector<card_id> peek_draw_pile(size_t n);
        void draw(size_t n);

        void trash(std::vector<card_id> &range, size_t trash_idx);
        void discard(std::vector<card_id> &range, size_t trash_idx);
        void add(std::vector<card_id> &range, const card_id &card);

        void add_to_discard_pile(const card_id &card) { discard_pile.push_back(card); }

        void end_turn();

        shared::ReducedPlayer get_reduced_player();
        shared::ReducedEnemy get_reduced_enemy();

        auto &get_draw_pile() { return draw_pile; }
        auto &get_discard_pile() { return discard_pile; }
        auto &get_hand_cards() { return hand_cards; }
        auto &get_current_card() { return currently_playing_card; }
        auto &get_played_cards() { return played_cards; }

        void increase_actions(int n) { available_actions += n; }
        void increase_buys(int n) { available_buys += n; }
        void increase_treasure(int n) { available_treasure += n; }
        void add_points(int n) { victory_points += n; }

    private:
        void reset_values();
        void shuffle_deck();

        std::deque<card_id> draw_pile;
        std::vector<card_id> discard_pile;
        std::vector<card_id> hand_cards;
        std::vector<card_id> played_cards; // should probably in base player?

        card_id currently_playing_card;
        size_t current_behaviour_idx;
    };
} // namespace server