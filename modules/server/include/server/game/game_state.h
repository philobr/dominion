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

    std::string toString(GamePhase phase)
    {
        switch ( phase ) {
            case GamePhase::ACTION_PHASE:
                return "action phase";
            case GamePhase::BUY_PHASE:
                return "buy phase";
            case GamePhase::PLAYING_ACTION_CARD:
                return "currently playing phase";
            default:
                return "INVALID PHASE";
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
        void endGame() { return; }

        void initialisePlayers(const std::vector<Player::id_t> &player_ids);
        void initialiseBoard(const std::vector<shared::CardBase::id_t> &selected_cards);

        void startTurn();
        void endTurn();

        void setPhase(GamePhase new_phase) { phase = new_phase; }

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

        /**
         * @brief This function checks if the player is in Action phase, the card is an action card and if the
         * player has actions left. If both conditions are met, the card is moved from the hand/staged cards to
         * the played cards and the currently_playing_card (in Player) and the actions are decremented.
         */
        void tryBuy(const Player::id_t &player_id, const shared::CardBase::id_t &card);
        void tryPlayFromHand(const Player::id_t &affected_player, const shared::CardBase::id_t &card_id);

        /**
         * @brief Switches phases if necessary, this means: if a player is out of buys or out of actions
         * (im not happy with this name but i got nothing better rn)
         */
        void maybeSwitchPhase();

        void tryEndActionPhase(const shared::PlayerBase::id_t &requestor_id)
        {
            if ( requestor_id != getCurrentPlayerId() ) {
                LOG(WARN) << requestor_id << " tried to end ActionPhase, but he is currently not playing";
                throw exception::InvalidRequest("It's not your turn!");
            }

            if ( phase != GamePhase::ACTION_PHASE ) {
                LOG(WARN) << requestor_id << " tried to end ActionPhase, but he is currently not playing";
                throw exception::OutOfPhase("Can't end " + toString(GamePhase::ACTION_PHASE) + " while beeing in " +
                                            toString(phase));
            }

            forceSwitchPhase();
        }

        void tryBuy(const shared::PlayerBase::id_t &requestor_id, const shared::CardBase::id_t &card_id)
        {
            if ( requestor_id != getCurrentPlayerId() ) {
                LOG(WARN) << requestor_id << " tries to buy a card but he is currently not playing";
                throw exception::InvalidRequest("It's not your turn!");
            }

            if ( phase != GamePhase::BUY_PHASE ) {
                LOG(WARN) << requestor_id << " buys a card, but he is not in " << toString(GamePhase::BUY_PHASE);
                throw exception::OutOfPhase("Can't buys a card " + toString(GamePhase::BUY_PHASE) +
                                            " while beeing in " + toString(phase));
            }

            auto player = getPlayer(requestor_id);
            const auto card_cost = shared::CardFactory::getCard(card_id).getCost();

            if ( !player.canBuy(card_cost) ) {
                LOG(WARN) << requestor_id << " can not afford the card " << card_id << " (costs: " << card_cost
                          << ", has: " << player.getTreasure()
                          << "), or he is out of buys (needs: 1, has: " << player.getBuys() << ")";
                throw exception::InsufficientFunds();
            }

            board->tryTake(card_id);

            player.decTreasure(card_cost);
            player.decBuys();
            player.gain(card_id);

            LOG(INFO) << "player: " << requestor_id << " successfully bought a new card with id: " << card_id;
        }

        void tryGain(const shared::PlayerBase::id_t &requestor_id, const shared::CardBase::id_t &card_id)
        {
            if ( phase != GamePhase::PLAYING_ACTION_CARD ) {
                LOG(WARN) << requestor_id << " tries to gain a card, but he is not in "
                          << toString(GamePhase::PLAYING_ACTION_CARD);
                throw exception::OutOfPhase("Can't gain a card while beeing in " + toString(phase));
            }

            board->tryTake(card_id);

            auto player = getPlayer(requestor_id);
            player.gain(card_id);

            LOG(INFO) << "player: " << requestor_id << " successfully gained a new card with id: " << card_id;
        }

        void tryPlayFromHand(const shared::PlayerBase::id_t &requestor_id, const shared::CardBase::id_t &card_id)
        {
            if ( requestor_id != getCurrentPlayerId() ) {
                LOG(WARN) << requestor_id << " tried to call " << FUNC_NAME << ", but he is currently not playing!";
                throw exception::InvalidRequest("It's not your turn!");
            }

            if ( phase != GamePhase::ACTION_PHASE ) {
                LOG(WARN) << requestor_id << " tried to play a card with id: " << card_id
                          << " from his hand, but he is not in " << toString(GamePhase::ACTION_PHASE);
                throw exception::OutOfPhase("Can't play a card while beeing in " + toString(phase));
            }

            auto player = getPlayer(requestor_id);
            if ( !player.hasCardInHand(card_id) ) {
                LOG(WARN) << requestor_id << " tried to play card: " << card_id
                          << " from his hand cards, but he does not have it";
                throw exception::CardNotAvailable();
            }

            // tryPlay
        }

        void tryPlayFromStaged(const shared::PlayerBase::id_t &requestor_id, const shared::CardBase::id_t &card_id)
        {
            if ( requestor_id != getCurrentPlayerId() ) {
                LOG(WARN) << requestor_id << " tried to call " << FUNC_NAME << ", but he is currently not playing!";
                throw exception::InvalidRequest("It's not your turn!");
            }
        }

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
