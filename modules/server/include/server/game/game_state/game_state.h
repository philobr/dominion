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

        std::unique_ptr<reduced::GameState> getReducedState(const Player::id_t &affected_player);

        const Player::id_t &getCurrentPlayerId() const { return player_order[current_player_idx]; }
        Player &getCurrentPlayer() { return *player_map[getCurrentPlayerId()]; }
        Player &getPlayer(const Player::id_t &id) { return *player_map.at(id); }

        GamePhase getPhase() const { return phase; }

        void startGame();
        void endGame() { return; }

        void initialisePlayers(const std::vector<Player::id_t> &player_ids);
        void initialiseBoard(const std::vector<shared::CardBase::id_t> &selected_cards);

        void startTurn();
        void endTurn();

        bool isGameOver() const;

        /**
         * @brief As of now, this function tries to buy a card for the given player.
         * It throws exception::InsufficientFunds or exception::CardNotAvailable accordingly.
         *
         * @param player_id
         * @param card
         * @return true
         * @return false
         */
        bool tryBuy(const Player::id_t &player_id, const shared::CardBase::id_t &card);
        bool tryPlay(const Player::id_t &affected_player, size_t hand_index, size_t behaviour_index);

        /**
         * @brief Switches phases if necessary, this means: if a player is out of buys or out of actions
         * (im not happy with this name but i got nothing better rn)
         */
        void maybeSwitchPhase();

    private:
        /**
         * @brief Forces a phase switch. This is called if a player ends a phase early
         */
        void forceSwitchPhase();

        void resetPhase() { phase = GamePhase::ACTION_PHASE; }
        void switchPlayer() { current_player_idx = (current_player_idx + 1) % player_map.size(); }

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
