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
            throw exception::UnreachableCode();
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
            game_state->tryPlay<shared::CardAccess::HAND>(requestor_id, action_decision->card_id);
            // phase is only set if we successfully played a card
            game_state->setPhase(shared::GamePhase::PLAYING_ACTION_CARD);
        } catch ( exception::UnreachableCode &e ) {
            LOG(ERROR) << "Received unresolvable Error in " << FUNC_NAME << " aborting.";
            throw e;
        } catch ( std::exception &e ) {
            LOG(DEBUG) << "Failed to play card for player \'" << requestor_id << "\'. Error: " << e.what();
            throw e;
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
            game_state->tryBuy(requestor_id, action_decision->card);
        } catch ( exception::UnreachableCode &e ) {
            LOG(ERROR) << "Received unresolvable Error in " << FUNC_NAME << " aborting.";
            throw e;
        } catch ( std::exception &e ) {
            LOG(DEBUG) << "Failed to buy card for player \'" << requestor_id << "\'. Error: " << e.what();
            throw e;
        }

        return nextPhase();
    }

    GameInterface::response_t
    GameInterface::EndTurnDecision_handler(std::unique_ptr<shared::EndTurnDecision> action_decision,
                                           const Player::id_t &requestor_id)
    {
        try {
            game_state->tryEndTurn(requestor_id);
        } catch ( exception::UnreachableCode &e ) {
            LOG(ERROR) << "Somehow reached unreachable code in " << FUNC_NAME << ". Aborting.";
            throw e;
        } catch ( std::exception &e ) {
            LOG(WARN) << "Failed to end turn: " << e.what();
            throw e;
        }

        if ( game_state->isGameOver() ) {
            return endGame();
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
            LOG(WARN) << "Player: \'" << message->player_id << "\' called " << FUNC_NAME << ". Expected to be in \'"
                      << toString(shared::GamePhase::PLAYING_ACTION_CARD) << "\', but current phase is \'"
                      << toString(game_state->getPhase());
            throw exception::OutOfPhase("You can not do this while being in " + toString(game_state->getPhase()));
        }

        auto decision = std::move(message->decision);

        if ( (dynamic_cast<shared::DeckChoiceDecision *>(decision.get()) == nullptr) &&
             (dynamic_cast<shared::GainFromBoardDecision *>(decision.get()) == nullptr) ) {
            LOG(ERROR) << "Unreachable code: received some unexpected decision type in: " << FUNC_NAME;
            throw exception::UnreachableCode();
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
        } catch ( exception::UnreachableCode &e ) {
            LOG(ERROR) << "Somehow reached unreachable code in " << FUNC_NAME << ". Aborting.";
            throw e;
        } catch ( std::exception &e ) {
            LOG(WARN) << "Failed to end action phase: " << e.what();
            throw e;
        }

        return nextPhase();
    }

    GameInterface::response_t GameInterface::endGame()
    {
        GameInterface::response_t response;
        LOG(DEBUG) << "Game is over, sending results";
        std::vector<shared::PlayerResult> results = game_state->getResults();
        response.setGameOver(std::move(results));
        return response;
    }

    GameInterface::response_t GameInterface::nextPhase(bool end_turn)
    {
        auto current_player_id = game_state->getCurrentPlayerId();
        game_state->maybeSwitchPhase();
        if ( current_player_id != game_state->getCurrentPlayerId() && game_state->isGameOver() ) {
            // the player has changed in switchPhase and the game is over, hence the game has ended
            return endGame();
        }

        current_player_id = game_state->getCurrentPlayerId();
        switch ( game_state->getPhase() ) {
            case shared::GamePhase::ACTION_PHASE:
                return {current_player_id, std::make_unique<shared::ActionPhaseOrder>()};
            case shared::GamePhase::BUY_PHASE:
                {
                    for ( const auto &card_id : game_state->tryPlayAllTreasures(current_player_id) ) {
                        behaviour_chain->loadBehaviours(card_id);
                        behaviour_chain->startChain(*game_state);
                    }

                    return {current_player_id, std::make_unique<shared::BuyPhaseOrder>()};
                }
            case shared::GamePhase::PLAYING_ACTION_CARD:
            default:
                {
                    LOG(ERROR) << "game_state is out of phase";
                    throw exception::UnreachableCode();
                }
                break;
        }
    }

    GameInterface::response_t GameInterface::finishedPlayingCard()
    {
        if ( game_state->getPhase() != shared::GamePhase::PLAYING_ACTION_CARD ) {
            LOG(ERROR) << "Tried to finish playing a card while not even playing a card!";
            throw exception::UnreachableCode();
        }

        game_state->setPhase(shared::GamePhase::ACTION_PHASE);
        return nextPhase();
    }
} // namespace server
