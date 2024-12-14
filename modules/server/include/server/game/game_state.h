#pragma once

#include <map>
#include <memory>
#include <vector>

#include <server/game/server_board.h>
#include <server/game/server_player.h>

#include <server/network/message_interface.h>
#include <shared/action_decision.h>

#include <shared/game/cards/card_base.h>
#include <shared/game/game_state/game_phase.h>
#include <shared/game/game_state/player_base.h>
#include <shared/game/game_state/reduced_game_state.h>

namespace server
{

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
        shared::GamePhase phase;
        bool is_actually_over = false;

    public:
        GameState();
        GameState(const std::vector<shared::CardBase::id_t> &play_cards, const std::vector<Player::id_t> &player_ids);
        ~GameState();
        GameState(GameState &&other);

        void initialisePlayers(const std::vector<Player::id_t> &player_ids);
        void initialiseBoard(const std::vector<shared::CardBase::id_t> &selected_cards);

#pragma region TRY_FUNCTIONS

        /**
         * @brief Ends the action phase if possible
         * @throws exception::InvalidRequest, exception::OutOfPhase
         */
        void tryEndActionPhase(const shared::PlayerBase::id_t &requestor_id);

        inline void tryEndTurn(const shared::PlayerBase::id_t &requestor_id)
        {
            guaranteeIsCurrentPlayer(requestor_id, FUNC_NAME);
            guaranteeNotPhase(requestor_id, shared::GamePhase::PLAYING_ACTION_CARD, "Can not end turn", FUNC_NAME);

            endTurn();
        }

        /**
         * @brief Buys a card from the board and adds it to the players discard pile.
         * @throws exception::InvalidRequest, exception::OutOfPhase, exception::InsufficientFunds
         */
        void tryBuy(const shared::PlayerBase::id_t &requestor_id, const shared::CardBase::id_t &card_id);

        /**
         * @brief Tries to play all treasures from a players hand.
         * @return All treasure cards in a players hand
         */
        std::vector<shared::CardBase::id_t> tryPlayAllTreasures(const shared::PlayerBase::id_t &requestor_id);

        /**
         * @brief Tries to play the given card_id from the specified pile.
         */
        template <enum shared::CardAccess FROM>
        inline void tryPlay(const shared::PlayerBase::id_t &requestor_id, const shared::CardBase::id_t &card_id);

        /**
         * @brief Tries to gain the given card_id to the given pile.
         */
        template <enum shared::CardAccess TO>
        inline void tryGain(const shared::PlayerBase::id_t &requestor_id, const shared::CardBase::id_t &card_id);

#pragma region GETTERS / SETTERS

        std::unique_ptr<reduced::GameState> getReducedState(const Player::id_t &affected_player);

        shared::GamePhase getPhase() const { return phase; }
        ServerBoard::ptr_t getBoard() { return board; }

        const Player::id_t &getCurrentPlayerId() const { return player_order[current_player_idx]; }
        Player &getCurrentPlayer() { return *player_map[getCurrentPlayerId()]; }

        Player &getPlayer(const Player::id_t &id) { return *player_map.at(id); }
        const Player &getPlayer(const Player::id_t &id) const { return *player_map.at(id); }

        inline void setPhase(shared::GamePhase new_phase) { phase = new_phase; }

        void endTurn();

        /**
         * @return vector containing all player ids
         */
        const std::vector<Player::id_t> &getAllPlayerIDs() const { return player_order; }

        /**
         * @brief Returns a vector containing the enemies of the calling player_id. The enemies are provided in playing
         * order.
         * @return vector containing the enemies, where vector[0]=next player in sequence
         */
        std::vector<Player::id_t> getEnemyIDs(const Player::id_t &player_id) const;

        /**
         * @return true; if the game is over
         */
        bool isGameOver() const { return board->isGameOver() && (is_actually_over); }

        /**
         * @brief If the game has ended, this will return the results of the game.
         *
         * The results are sorted by the amount of victory points each player has.
         *
         * This should not be called before the game has ended.
         */
        std::vector<shared::PlayerResult> getResults() const;

        /**
         * @brief Switches phases if necessary, this means: if a player is out
         * of buys or out of actions
         * (im not happy with this name but i got nothing better rn)
         *
         * Note that this function might also switch the player if the current
         * player has ended their turn.
         *
         * @return true if the player's turn has ended.
         */
        void maybeSwitchPhase();

    private:
        /**
         * @brief Forces a phase switch. This is called if a player ends a phase early
         */
        void forceSwitchPhase();

        inline void resetPhase() { phase = shared::GamePhase::ACTION_PHASE; }
        inline void switchPlayer() { current_player_idx = (current_player_idx + 1) % player_map.size(); }

#pragma region ASSERTION_HELPERS
        void printSuccess(const shared::PlayerBase::id_t &requestor_id, const std::string &function_name);
        void guaranteePhase(const shared::PlayerBase::id_t &requestor_id, const shared::CardBase::id_t &card_id,
                            shared::GamePhase expected_phase, const std::string &error_msg,
                            const std::string &function_name);

        void guaranteePhase(const shared::PlayerBase::id_t &requestor_id, shared::GamePhase expected_phase,
                            const std::string &error_msg, const std::string &function_name);

        void guaranteeNotPhase(const shared::PlayerBase::id_t &requestor_id, const shared::CardBase::id_t &card_id,
                               shared::GamePhase expected_phase, const std::string &error_msg,
                               const std::string &function_name);

        void guaranteeNotPhase(const shared::PlayerBase::id_t &requestor_id, shared::GamePhase expected_phase,
                               const std::string &error_msg, const std::string &function_name);

        void guaranteeIsCurrentPlayer(const shared::PlayerBase::id_t &requestor_id, const std::string &function_name);
    };

#include "game_state.hpp"
} // namespace server
