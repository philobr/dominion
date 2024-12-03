#include <server/game/game_interface.h>
#include <shared/utils/logger.h>
namespace server
{
    GameInterface::ptr_t GameInterface::make(const std::string &game_id,
                                             const std::vector<shared::CardBase::id_t> &play_cards,
                                             const std::vector<Player::id_t> &player_ids)
    {
        LOG(DEBUG) << "Created a new GameInterface("
                   << "game_id:" << game_id << ")";
        return ptr_t(new GameInterface(game_id, play_cards, player_ids));
    }

    GameInterface::response_t GameInterface::handleMessage(std::unique_ptr<shared::ClientToServerMessage> &message)
    {
        auto *casted_msg = dynamic_cast<shared::ActionDecisionMessage *>(message.get());
        if ( casted_msg == nullptr ) {
            // ISSUE: 166
            LOG(ERROR) << "Received a non shared::ActionDecisionMessage in " << FUNC_NAME;
            throw std::runtime_error("unreachable code");
        }

#define HANDLE_ACTION(type)                                                                                            \
    if ( dynamic_cast<shared::type *>(casted_msg->decision.get()) ) {                                                  \
        return type##_handler(                                                                                         \
                std::unique_ptr<shared::type>(static_cast<shared::type *>(casted_msg->decision.release())),            \
                casted_msg->player_id);                                                                                \
    }

        HANDLE_ACTION(PlayActionCardDecision);
        HANDLE_ACTION(BuyCardDecision);
        HANDLE_ACTION(EndTurnDecision);
        HANDLE_ACTION(ChooseNCardsFromHandDecision);

        LOG(ERROR) << "Unreachable code: received some weird message type";
        throw std::runtime_error("unreachable code in " + FUNC_NAME);
    }

    GameInterface::response_t
    GameInterface::PlayActionCardDecision_handler(std::unique_ptr<shared::PlayActionCardDecision> action_decision,
                                                  const Player::id_t &player_id)
    {
        try {
            // all checks are done here
            game_state->tryPlayFromHand(player_id, action_decision->card_id);
            game_state->setPhase(GamePhase::PLAYING_ACTION_CARD); // phase is only set if we successfully played a card
        } catch ( std::exception &e ) {
            // we throw for now, but this should be a message
            // discuss with gui guys or return shared::ResultResponseMessage(false)
            // ISSUE: 166
            LOG(ERROR) << "failed to play, TODO: handle this more gracefully";
            throw std::runtime_error("failed to play card (GameInterface::PlayActionCardDecision_handler), this needs "
                                     "to be handled better");
        }

        behaviour_chain->loadBehaviours(action_decision->card_id);
        auto response = behaviour_chain->startChain(*game_state);

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
            // all checks are done here
            game_state->tryBuy(player_id, action_decision->card);
        } catch ( std::exception &e ) {
            // we throw for now, but this should be a message
            // discuss with gui guys or return shared::ResultResponseMessage(false)
            // ISSUE: 166
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
            // ISSUE: 166
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

        LOG(ERROR) << FUNC_NAME << " is not implemented yet!";
        throw std::runtime_error("unrachable code");

        // auto order_msg = behaviour_chain->continueChain(*game_state, action_decision);
        if ( behaviour_chain->empty() ) {
            return finishedPlayingCard();
        } else {
            // return empty order
            return OrderResponse();
        }
    }

    GameInterface::response_t GameInterface::nextPhase()
    {
        // switches phase if: actions==0 OR (buys==0 -> end_turn + next player)
        game_state->maybeSwitchPhase();
        switch ( game_state->getPhase() ) {
            case server::GamePhase::ACTION_PHASE:
                return {game_state->getCurrentPlayerId(), std::make_unique<shared::ActionPhaseOrder>()};
            case server::GamePhase::BUY_PHASE:
                return {game_state->getCurrentPlayerId(), std::make_unique<shared::BuyPhaseOrder>()};
            case server::GamePhase::PLAYING_ACTION_CARD:
            default:
                {
                    // ISSUE: 166
                    LOG(ERROR) << "game_state is out of phase";
                    throw std::runtime_error("unreachable code");
                }
                break;
        }
    }

    GameInterface::response_t GameInterface::finishedPlayingCard()
    {
        if ( game_state->getPhase() != server::GamePhase::PLAYING_ACTION_CARD ) {
            // ISSUE: 166
            LOG(ERROR) << "tried to finish playing a card while not even playing a card!";
            throw std::runtime_error("unreachable code in " + FUNC_NAME);
        }

        game_state->setPhase(server::GamePhase::ACTION_PHASE);
        return nextPhase();
    }

} // namespace server
