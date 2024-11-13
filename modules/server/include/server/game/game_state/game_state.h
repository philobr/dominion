#pragma once

#include <map>
#include <memory>
#include <vector>

#include <server/game/game_state/server_board.h>
#include <server/game/game_state/server_player.h>

#include <server/server_network_manager.h>
#include <shared/action_decision.h>

#include <shared/game/cards/card_base.h>
#include <shared/game/game_state/player_base.h>
#include <shared/game/game_state/reduced_game_state.h>

namespace server
{
    class MessageInterface;
    // forward declaration
    class ServerBoard;

    /**
     * @brief This holds the complete game stae on the server.
     *
     * @see shared::ReducedGameState
     */
    class GameState
    {
        std::map<Player::id_t, Player::ptr_t> player_map;
        std::vector<Player::id_t> player_order;
        unsigned int current_player_idx;

        std::unique_ptr<server::ServerBoard> board;

    public:
        GameState();
        GameState(const std::vector<shared::CardBase::id_t> &play_cards, const std::vector<Player::id_t> &player_ids);
        ~GameState();
        GameState(GameState &&other);

        // probably nume das
        // either return message, or construct through msg interface
        // NOT IMPLEMENTED, DONT TEST YET
        void receive_action(std::unique_ptr<shared::ActionDecision> action, MessageInterface &message_interface);

        const Player::id_t &get_current_player_id() const { return player_order[current_player_idx]; }
        Player &get_current_player() { return *player_map[get_current_player_id()]; }
        Player &get_player(const Player::id_t &id) { return *player_map.at(id); }

    private:
        shared::ReducedGameState get_reduced_state(const Player::id_t &affected_player);

        void start_game();
        // NOT IMPLEMENTED, DONT TEST YET
        void end_game() { return; }

        void initialise_players(const std::vector<Player::id_t> &player_ids);
        void initialise_board(const std::vector<shared::CardBase::id_t> &selected_cards);

        void start_turn();
        void end_turn();

        void switch_player() { current_player_idx = ++current_player_idx % player_map.size(); }
        bool is_game_over() const;

        bool try_buy(const Player::id_t player_id, const shared::CardBase::id_t &card);
        bool try_play(const Player::id_t &affected_player, size_t hand_index, size_t behaviour_index);
    };

} // namespace server

// this is down here because the cards need the gamestate at compiletime, but the gamestate does not need the cards at
// compiletime
#include <server/game/cards/cards.h>