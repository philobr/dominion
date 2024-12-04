#pragma once

#include <map>
#include <memory>
#include <vector>

#include <server/game/server_board.h>
#include <server/game/server_player.h>

#include <server/network/message_interface.h>
#include <shared/action_decision.h>

#include <shared/game/cards/card_base.h>
#include <shared/game/game_state/player_base.h>
#include <shared/game/game_state/reduced_game_state.h>

namespace server
{
    enum class GamePhase
    {
        ACTION_PHASE,
        BUY_PHASE,
        PLAYING_ACTION_CARD,
    };

    inline std::string toString(GamePhase phase)
    {
        switch ( phase ) {
            case GamePhase::ACTION_PHASE:
                return "action phase";
            case GamePhase::BUY_PHASE:
                return "buy phase";
            case GamePhase::PLAYING_ACTION_CARD:
                return "currently playing phase";
            default:
                {
                    LOG(ERROR) << "Received unexpected phase in " << FUNC_NAME << ": " << static_cast<int>(phase);
                    return "INVALID PHASE";
                }
        }
    }

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
        const Player &getPlayer(const Player::id_t &id) const { return *player_map.at(id); }

        GamePhase getPhase() const { return phase; }

        void startGame();
        void endGame()
        {
            LOG(ERROR) << FUNC_NAME << " is not implemented lol";
            return;
        }

        void initialisePlayers(const std::vector<Player::id_t> &player_ids);
        void initialiseBoard(const std::vector<shared::CardBase::id_t> &selected_cards);

        void startTurn();
        void endTurn();

        inline void setPhase(GamePhase new_phase) { phase = new_phase; }

        bool isGameOver() const;

        std::vector<shared::CardBase::id_t> playAllTreasures(const shared::PlayerBase::id_t &affected_player_id)
        {
            auto cards_to_play =
                    getPlayer(affected_player_id).getType<shared::CardAccess::HAND>(shared::CardType::TREASURE);
            play(affected_player_id, cards_to_play);
            return cards_to_play;
        }

        void play(const shared::PlayerBase::id_t &affected_player_id, const shared::CardBase::id_t &card_id)
        {
            getPlayer(affected_player_id).playCardFromHand(card_id);
            board->addToPlayedCards(card_id);
        }

        void play(const shared::PlayerBase::id_t &affected_player_id,
                  const std::vector<shared::CardBase::id_t> &card_ids)
        {
            for ( const auto &card_id : card_ids ) {
                play(affected_player_id, card_id);
            }
        }

        /**
         * @brief Switches phases if necessary, this means: if a player is out of buys or out of actions
         * (im not happy with this name but i got nothing better rn)
         */
        void maybeSwitchPhase();

        /**
         * @brief Ends the action phase if possible
         * @throws exception::InvalidRequest, exception::OutOfPhase
         */
        void tryEndActionPhase(const shared::PlayerBase::id_t &requestor_id);

        /**
         * @brief Buys a card from the board and adds it to the players discard pile.
         * @throws exception::InvalidRequest, exception::OutOfPhase, exception::InsufficientFunds
         */
        void tryBuy(const shared::PlayerBase::id_t &requestor_id, const shared::CardBase::id_t &card_id);

        /**
         * @brief Takes a card from the board (without payment)
         * @throws exception::OutOfPhase, exception::CardNotAvailable
         */
        void tryGain(const shared::PlayerBase::id_t &requestor_id, const shared::CardBase::id_t &card_id);

        /**
         * @brief Plays a card from the hand cards. This involves moving the card from the hand to played cards.
         * @throws exception::InvalidRequest, exception::OutOfPhase, exception::OutOfActions,
         * exception::CardNotAvailable
         */
        void tryPlayFromHand(const shared::PlayerBase::id_t &requestor_id, const shared::CardBase::id_t &card_id);

        /**
         * @brief Plays a card from the staged cards. This involves moving the card from staged to played cards.
         * @throws exception::InvalidRequest, exception::OutOfPhase, exception::CardNotAvailable
         */
        void tryPlayFromStaged(const shared::PlayerBase::id_t &requestor_id, const shared::CardBase::id_t &card_id);

    private:
        /**
         * @brief Forces a phase switch. This is called if a player ends a phase early
         */
        void forceSwitchPhase();

        inline void resetPhase() { phase = GamePhase::ACTION_PHASE; }
        inline void switchPlayer();

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
