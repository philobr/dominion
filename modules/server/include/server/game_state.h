#pragma once

#include <deque>
#include <memory>
#include <vector>

#include "shared/game_state.h"
#include "shared/message_types.h"

#include "message_interface.h"

namespace server
{

    /**
     * @brief ServerBoard is a class that represents the game board of the server.
     *
     * The ServerBoard offers additional functionality on top of the shared::Board class.
     */
    class ServerBoard : public shared::Board
    {
    public:
        bool buy(shared::CardBase::id_t card);
        void trash(shared::CardBase::id_t card);
    };

    /**
     * @brief Player represents a player on the server (obviously ;) ).
     *
     * This class holds all the information about a player that is needed on the server.
     */
    class Player : public shared::PlayerBase
    {
    public:
        Player() {};
        Player(shared::PlayerBase::id_t id) : shared::PlayerBase(id) {};
        std::deque<shared::CardBase::id_t> draw_pile;
        std::vector<shared::CardBase::id_t> discard_pile;
        std::vector<shared::CardBase::id_t> hand_cards;
        shared::CardBase::id_t currently_playing_card;
    };

    /**
     * @brief This holds the complete game state on the server.
     *
     * @see shared::ReducedGameState
     */
    class GameState
    {
    public:
        GameState() {};
        void receive_action(std::unique_ptr<shared::ActionDecisionMessage> action, MessageInterface &message_interface);
        shared::ReducedGameState get_reduced_state(shared::PlayerBase::id_t player);
        void add_player(Player player);
        const std::vector<Player> &get_players();
        void start_game(std::vector<shared::CardBase::id_t> selected_cards) { return; }; // TODO implement

    private:
        void end_game();
        void start_turn();
        void end_turn();
        void switch_player(shared::PlayerBase::id_t player);
        bool try_buy(shared::PlayerBase::id_t player, shared::CardBase::id_t card);
        bool trash(shared::PlayerBase::id_t player, shared::CardBase::id_t card);
        bool discard(shared::PlayerBase::id_t player, shared::CardBase::id_t card);
        bool draw(shared::PlayerBase::id_t player, unsigned int count);
        bool play(shared::PlayerBase::id_t player, unsigned int index);
        bool is_game_over();

        std::vector<Player> players;
        ServerBoard board;
        shared::PlayerBase::id_t current_player;
    };
} // namespace server
