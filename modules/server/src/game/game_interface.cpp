#include <server/game/game_interface.h>
#include <shared/utils/logger.h>
namespace server
{
    GameInterface::ptr_t GameInterface::make(const std::string &game_id,
                                             const std::vector<shared::CardBase::id_t> &play_cards,
                                             const std::vector<Player::id_t> &player_ids)
    {
        LOG(INFO) << "Created a new GameInterface("
                  << "game_id:" << game_id << ")";
        return ptr_t(new GameInterface(game_id, play_cards, player_ids));
    }

    GameInterface::response_t GameInterface::handleMessage(std::unique_ptr<shared::ClientToServerMessage> &message)
    {
        auto *casted_msg = dynamic_cast<shared::ActionDecisionMessage *>(message.get());
        if ( casted_msg == nullptr ) {
            LOG(ERROR) << "Received a non shared::ActionDecisionMessage in " << FUNC_NAME;
            throw std::runtime_error("unreachable code");
        }

        auto affected_player_id = casted_msg->player_id;
        return handleAction(std::move(casted_msg->decision), affected_player_id);
    }

    GameInterface::response_t GameInterface::handleAction(std::unique_ptr<shared::ActionDecision> action_decision,
                                                          const Player::id_t &affected_player_id)
    {
#define HANDLE_ACTION(type)                                                                                            \
    if ( dynamic_cast<shared::type *>(action_decision.get()) ) {                                                       \
        return type##_handler(std::unique_ptr<shared::type>(static_cast<shared::type *>(action_decision.release())),   \
                              affected_player_id);                                                                     \
    }
        HANDLE_ACTION(PlayActionCardDecision);
        HANDLE_ACTION(BuyCardDecision);
        HANDLE_ACTION(EndTurnDecision);
        HANDLE_ACTION(ChooseNCardsFromHandDecision);

        LOG(ERROR) << "Unreachable code: received some weird message type";
        throw std::runtime_error("Unreachable code");
    }

    GameInterface::response_t
    GameInterface::PlayActionCardDecision_handler(std::unique_ptr<shared::PlayActionCardDecision> action_decision,
                                                  const Player::id_t &player_id)
    {
        try {
            game_state->tryPlay(player_id, action_decision->card_id, action_decision->from);
            game_state->setPhase(GamePhase::PLAYING_ACTION_CARD); // phase is only set if we successfully played a card
        } catch ( std::exception &e ) {
            LOG(ERROR) << "failed to play, TODO: handle this";
            throw std::runtime_error("failed to play card (GameInterface::PlayActionCardDecision_handler), this needs "
                                     "to be handled better");
        }

        behaviour_chain->loadBehaviours(action_decision->card_id);
        auto response = behaviour_chain->start(*game_state);

        // we are done playing this card
        if ( behaviour_chain->empty() ) {
            return finishedPlayingCard();
        }

        return response;
    }

    GameInterface::response_t
    GameInterface::BuyCardDecision_handler(std::unique_ptr<shared::BuyCardDecision> action_decision,
                                           const Player::id_t &player_id)
    {
        try {
            game_state->tryBuy(player_id, action_decision->card);
        } catch ( std::exception &e ) {
            LOG(ERROR) << "failed to buy card";
            throw std::runtime_error("failed to buy in GameInterface::PlayActionCardDecision_handler, this needs to be "
                                     "handled better");
        }

        return nextPhase();
    }

    GameInterface::response_t
    GameInterface::EndTurnDecision_handler(std::unique_ptr<shared::EndTurnDecision> action_decision,
                                           const Player::id_t &player_id)
    {
        if ( game_state->getPhase() == GamePhase::PLAYING_ACTION_CARD ) {
            LOG(ERROR) << "Player is trying to end his turn while playing a card";
            throw exception::OutOfPhase("");
        }

        game_state->endTurn();

        return nextPhase();
    }

    GameInterface::response_t GameInterface::ChooseNCardsFromHandDecision_handler(
            std::unique_ptr<shared::ChooseNCardsFromHandDecision> action_decision, const Player::id_t &player_id)
    {
        // this will be implemented after ChooseNCardFromHandDecision is fixed

        // auto order_msg = behaviour_chain->receiveAction(*game_state, action_decision);
        //
        // if ( !order_msg.empty() ) {
        //     return order_msg;
        // }
        //
        // return finishedPlayingCard();

        LOG(ERROR) << FUNC_NAME << " is not implemented yet!";
        throw std::runtime_error("unrachable code");

        if ( behaviour_chain->empty() ) {
            return finishedPlayingCard();
        } else {
            // return empty order
            return OrderResponse();
        }
    }

} // namespace server
