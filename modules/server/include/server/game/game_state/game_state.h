#pragma once

#include <map>
#include <memory>
#include <vector>

#include <server/game/game_state/server_board.h>
#include <server/game/game_state/server_player.h>

#include <server/network/message_interface.h>
#include <shared/action_decision.h>

#include <shared/game/cards/card_base.h>
#include <shared/game/game_state/player_base.h>
#include <shared/game/game_state/reduced_game_state.h>

namespace server
{
    class MessageInterface;
    // forward declaration
    class ServerBoard;

    enum class GamePhase
    {
        ACTION_PHASE,
        BUY_PHASE,
        PLAYING_ACTION_CARD,
    };

    /**
     * @brief This holds the complete game stae on the server.
     *
     * @see reduced::GameState
     */
    class GameState
    {
        std::map<Player::id_t, Player::ptr_t> player_map;
        std::vector<Player::id_t> player_order;
        unsigned int current_player_idx;
        ServerBoard::ptr_t board;
        GamePhase phase;

    public:
        GameState();
        GameState(const std::vector<shared::CardBase::id_t> &play_cards, const std::vector<Player::id_t> &player_ids);
        ~GameState();
        GameState(GameState &&other);

        std::unique_ptr<reduced::GameState> get_reduced_state(const Player::id_t &affected_player);

        const Player::id_t &getCurrentPlayerId() const { return player_order[current_player_idx]; }
        Player &get_current_player() { return *player_map[getCurrentPlayerId()]; }
        Player &get_player(const Player::id_t &id) { return *player_map.at(id); }

        GamePhase getPhase() const { return phase; }

        void start_game();
        void end_game() { return; }

        void initialise_players(const std::vector<Player::id_t> &player_ids);
        void initialise_board(const std::vector<shared::CardBase::id_t> &selected_cards);

        void start_turn();
        void end_turn();

        bool is_game_over() const;

        /**
         * @brief As of now, this function tries to buy a card for the given player.
         * It throws exception::InsufficientFunds or exception::CardNotAvailable accordingly.
         *
         * @param player_id
         * @param card
         * @return true
         * @return false
         */
        bool try_buy(const Player::id_t &player_id, const shared::CardBase::id_t &card);
        bool try_play(const Player::id_t &affected_player, size_t hand_index, size_t behaviour_index);

        /**
         * @brief Switches phases if necessary, this means: if a player is out of buys or out of actions
         * (im not happy with this name but i got nothing better rn)
         */
        void maybe_switch_phase();

    private:
        /**
         * @brief Forces a phase switch. This is called if a player ends a phase early
         */
        void force_switch_phase();

        void reset_phase() { phase = GamePhase::ACTION_PHASE; }
        void switch_player() { current_player_idx = (current_player_idx + 1) % player_map.size(); }

        /**
         * @brief Checks if all ids exist and if the CardType is one of:
         * shared::CardType::ACTION | shared::CardType::ATTACK | shared::CardType::REACTION
         *
         * @param kingdom_cards 10 valid kingdom cards id
         * @return true all cards exist and have valid type
         * @return false
         */
        static bool validateKingdomCardTypes(const std::vector<shared::CardBase::id_t> &kingdom_cards);
    };
} // namespace server
