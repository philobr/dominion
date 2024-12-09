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
        auto casted_msg = std::unique_ptr<shared::ActionDecisionMessage>(
                static_cast<shared::ActionDecisionMessage *>(message.release()));

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
        HANDLE_ACTION(EndActionPhaseDecision);

        return passToBehaviour(casted_msg);
    }

    GameInterface::response_t
    GameInterface::PlayActionCardDecision_handler(std::unique_ptr<shared::PlayActionCardDecision> action_decision,
                                                  const Player::id_t &requestor_id)
    {
        try {
            // all checks are done here
            game_state->tryPlayFromHand(requestor_id, action_decision->card_id);
            game_state->setPhase(
                    shared::GamePhase::PLAYING_ACTION_CARD); // phase is only set if we successfully played a card
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
                                           const Player::id_t &requestor_id)
    {
        try {
            // all checks are done here
            game_state->tryBuy(requestor_id, action_decision->card);
        } catch ( std::exception &e ) {
            // TODO we throw for now, but this should be a message
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
                                           const Player::id_t &requestor_id)
    {
        if ( requestor_id != game_state->getCurrentPlayerId() ) {
            LOG(ERROR) << "Player " << requestor_id << " is trying to end the turn but its not his turn!";
            throw exception::NotYourTurn();
        }

        if ( game_state->getPhase() == shared::GamePhase::PLAYING_ACTION_CARD ) {
            // ISSUE: 166
            LOG(ERROR) << "Player " << requestor_id << " is trying to end his turn while playing a card";
            throw exception::OutOfPhase("");
        }

        game_state->endTurn();

        if ( game_state->isGameOver() ) {
            LOG(DEBUG) << "Game is over, sending results";
            GameInterface::response_t response;
            std::vector<shared::PlayerResult> results = game_state->getResults();
            response.setGameOver(std::move(results));
            return response;
        } else {
            return nextPhase();
        }
    }

    /**
     * @brief This function is used for ActionDecisionMessages that are not handled by other handlers. Those are assumed
     * to be expected by an ongoing behaviour.
     */
    GameInterface::response_t GameInterface::passToBehaviour(std::unique_ptr<shared::ActionDecisionMessage> &message)
    {
        // we expect to be in this state because the behaviour chain needs to be initialised
        // -> implying we are playing a card
        if ( game_state->getPhase() != shared::GamePhase::PLAYING_ACTION_CARD ) {
            LOG(ERROR) << "Unexpected message type, player " << message->player_id
                       << " is currently not playing a card";
            throw exception::OutOfPhase("");
        }

        auto decision = std::move(message->decision);

        if ( (dynamic_cast<shared::DeckChoiceDecision *>(decision.get()) == nullptr) &&
             (dynamic_cast<shared::GainFromBoardDecision *>(decision.get()) == nullptr) ) {
            LOG(ERROR) << "Unreachable code: received some unexpected decision type in: " << FUNC_NAME;
            throw std::runtime_error("unreachable code in " + FUNC_NAME);
        }

        auto response = behaviour_chain->continueChain(*game_state, decision);

        if ( behaviour_chain->empty() ) {
            return finishedPlayingCard();
        }

        return response;
    }

    GameInterface::response_t
    GameInterface::EndActionPhaseDecision_handler(std::unique_ptr<shared::EndActionPhaseDecision> decision,
                                                  const Player::id_t &requestor_id)
    {
        try {
            game_state->tryEndActionPhase(requestor_id);
        } catch ( std::exception &e ) {
            LOG(ERROR) << "failed to end action phase: " << e.what();
            throw std::runtime_error("failed to buy in GameInterface::PlayActionCardDecision_handler, this needs to be "
                                     "handled better");
        }

        return nextPhase();
    }

    GameInterface::response_t GameInterface::nextPhase()
    {
        // switches phase if: actions==0 OR (buys==0 -> end_turn + next player)
        game_state->maybeSwitchPhase();
        const auto current_player = game_state->getCurrentPlayer();
        switch ( game_state->getPhase() ) {
            case shared::GamePhase::ACTION_PHASE:
                return {current_player.getId(), std::make_unique<shared::ActionPhaseOrder>()};
            case shared::GamePhase::BUY_PHASE:
                {
                    for ( const auto &card_id : game_state->playAllTreasures(current_player.getId()) ) {
                        behaviour_chain->loadBehaviours(card_id);
                        behaviour_chain->startChain(*game_state);
                    }

                    return {current_player.getId(), std::make_unique<shared::BuyPhaseOrder>()};
                }
            case shared::GamePhase::PLAYING_ACTION_CARD:
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
        if ( game_state->getPhase() != shared::GamePhase::PLAYING_ACTION_CARD ) {
            // ISSUE: 166
            LOG(ERROR) << "tried to finish playing a card while not even playing a card!";
            throw std::runtime_error("unreachable code in " + FUNC_NAME);
        }

        game_state->setPhase(shared::GamePhase::ACTION_PHASE);
        return nextPhase();
    }
} // namespace server
