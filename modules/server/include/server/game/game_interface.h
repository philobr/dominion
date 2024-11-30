#pragma once

#include <server/game/behaviour_chain.h>
#include <server/game/game_state.h>

namespace server
{
    class GameInterface
    {
        std::shared_ptr<GameState> game_state;
        std::shared_ptr<BehaviourChain> behaviour_chain;
        const std::string game_id;

    public:
        using ptr_t = std::unique_ptr<GameInterface>;
        using response_t = server::BehaviourChain::ret_t;

        GameInterface operator=(const GameInterface &other) = delete;
        GameInterface(const GameInterface &other) = delete;
        GameInterface(GameInterface &&other) = default;
        ~GameInterface() = default;

        static ptr_t make(const std::string &game_id, const std::vector<shared::CardBase::id_t> &play_cards,
                          const std::vector<Player::id_t> &player_ids);

        inline auto getGameState(const shared::PlayerBase::id_t &player_id)
        {
            return game_state->getReducedState(player_id);
        }

        /**
         * @brief Receives an ActionDecision from the Lobby and handles it accordingly.
         * It will return some sort of ServerToClient message, which the lobby manager can pass on.
         *
         * @warning THIS IS WORK IN PROGRESS, I WILL FIX THIS WITH A FOLLOW UP MERGE REQ
         *
         * @param action_decision
         * @param in_response_to
         * @param game_id
         * @param affected_player_id
         * @return response_t
         */
        response_t handleMessage(std::unique_ptr<shared::ClientToServerMessage> &action_decision);

    private:
        GameInterface(const std::string &game_id, const std::vector<shared::CardBase::id_t> &play_cards,
                      const std::vector<Player::id_t> &player_ids) :
            game_state(std::make_shared<GameState>(play_cards, player_ids)),
            behaviour_chain(std::make_unique<BehaviourChain>()), game_id(game_id)
        {}

        response_t nextPhase();
        response_t finishedPlayingCard();

        // TODO: expand this macro when the messages are finally final

#define HANDLER(decision_type) /* can also be used to define the func outside of the class */                          \
    response_t decision_type##_handler(std::unique_ptr<shared::decision_type> decision,                                \
                                       const Player::id_t &affected_player_id)

        HANDLER(PlayActionCardDecision);
        HANDLER(BuyCardDecision);
        HANDLER(EndTurnDecision);
        HANDLER(ChooseNCardsFromHandDecision);

    }; // namespace server
} // namespace server
