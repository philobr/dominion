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
        auto casted_msg = dynamic_cast<shared::ActionDecisionMessage *>(message.get());
        if ( casted_msg == nullptr ) {
            LOG(ERROR) << "Received a non shared::ActionDecisionMessage in " << FUNC_NAME;
            throw std::runtime_error("unreachable code");
        }

        if ( casted_msg->in_response_to.has_value() ) {
            LOG(ERROR) << "this is not implemented yet!";
            throw std::runtime_error("not implemented");
        }

        auto affected_player_id = casted_msg->player_id;
        return handleAction(std::move(casted_msg->decision), affected_player_id);

        /* This might become useful later on
         * TODO: use this or delete it
        return in_response_to.has_value()
                ? handle_response(std::move(action_decision), affected_player_id, in_response_to.value());
                : handle_action(std::move(action_decision), affected_player_id)
        */
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

        LOG(ERROR) << "This should not be reachable, i will self destruct now!";
        throw std::runtime_error("Unreachable code");
    }

    GameInterface::response_t GameInterface::handleResponse(std::unique_ptr<shared::ActionDecision> /*action_decision*/,
                                                            const std::string & /*in_response_to*/,
                                                            const Player::id_t & /*affected_player_id*/)
    {
#define HANDLE_RESPONSE(type)                                                                                          \
    if ( dynamic_cast<shared::type *>(action_decision.get()) ) {                                                       \
        return type##_response_handler(                                                                                \
                std::unique_ptr<shared::type>(static_cast<shared::type *>(action_decision.release())),                 \
                affected_player_id, in_response_to);                                                                   \
    }

        // HANDLE_RESPONSE(PlayActionCardDecision);
        // HANDLE_RESPONSE(BuyCardDecision);
        // HANDLE_RESPONSE(EndTurnDecision);
        // HANDLE_RESPONSE(ChooseNCardsFromHandDecision);

        LOG(ERROR) << "This should not be reachable, i will self destruct now!";
        throw std::runtime_error("Unreachable code");
    }

    GameInterface::response_t
    GameInterface::PlayActionCardDecision_handler(std::unique_ptr<shared::PlayActionCardDecision> action_decision,
                                                  const Player::id_t &player_id)
    {
        /*
        this should probably be hand_cards in the first call?

        if ( action_decision->cardIndex >=
             game_state->getPlayer(player_id).get<shared::CardAccess::PLAYED_CARDS>().size() ) {
            LOG(ERROR) << "player(" << player_id << ") tried to play a card that is not in his played cards";
            throw exception::InvalidCardAccess("");
        }
        */

        /*
        flow is:

        - is phase correct?

        - CAN this card be played? (exists?)
        - get idx

        try {
            play(player, card_type, from_where); -> only moves the card from the given pile to the played cards
        } catch (play_exception){
            return error_msg; (no throw)
        }

        load behaviours
        run them until return
            if nullopt:
                play_card_done(); // cleanup stuff, try phaseswitch etc etc
            else return order.

        // we also need a function that can keep running behaviours
        -> can it be that all decision_responses go back into behaviours?
        */


        if ( game_state->getPhase() != GamePhase::ACTION_PHASE ) {
            LOG(ERROR) << "player tried to play an action card out of phase";
            throw std::runtime_error("what should happen here?");
        }

        const auto played_card_idx = action_decision->cardIndex;
        if ( !game_state->canPlay(player_id, played_card_idx) ) {
            LOG(ERROR) << "player(" << player_id
                       << ") tried to play a card that is not in his hand (index:" << played_card_idx
                       << ", handsize: " << game_state->getPlayer(player_id).get<shared::CardAccess::HAND>().size()
                       << ")";
            throw exception::InvalidCardAccess("");
        }

        const auto &card_id = game_state->getCardId(player_id, played_card_idx);
        // checks if the card is currently playable (multiple checks done)
        if ( !game_state->tryPlay(player_id, action_decision->cardIndex, shared::CardAccess::HAND) ) {
            LOG(ERROR) << "failed to play card after all checks";
            throw std::runtime_error("unreachable code");
        }

        cur_behaviours->loadBehaviours(card_id);
        auto response = cur_behaviours->receiveAction(*game_state, player_id, std::nullopt, std::nullopt);
        if ( response.has_value() ) {
            return std::move(response.value());
        }

        game_state->maybeSwitchPhase();

        switch ( game_state->getPhase() ) {
            case server::GamePhase::ACTION_PHASE:
                return std::make_unique<shared::ActionPhaseOrder>();
            case server::GamePhase::BUY_PHASE:
                return std::make_unique<shared::BuyPhaseOrder>();
            case server::GamePhase::PLAYING_ACTION_CARD:
            default:
                {
                    LOG(ERROR) << "game_state is out of phase";
                    throw std::runtime_error("unreachable code");
                }
                break;
        }
    }

    GameInterface::response_t
    GameInterface::BuyCardDecision_handler(std::unique_ptr<shared::BuyCardDecision> action_decision,
                                           const Player::id_t &player_id)
    {
        if ( game_state->getPhase() != GamePhase::BUY_PHASE ) {
            LOG(WARN) << "player(" << player_id << ") is currently not in the buy phase, retrying";

            return std::make_unique<shared::BuyPhaseOrder>();
        }

        game_state->tryBuy(player_id, action_decision->card);

        // TODO: turn should end automatically if a player cant buy anymore
        return std::make_unique<shared::BuyPhaseOrder>();
    }

    GameInterface::response_t
    GameInterface::EndTurnDecision_handler(std::unique_ptr<shared::EndTurnDecision> /*action_decision*/,
                                           const Player::id_t &player_id)
    {
        if ( game_state->getPhase() == GamePhase::PLAYING_ACTION_CARD ) {
            LOG(ERROR) << "Player is trying to end his turn while playing a card";
            throw exception::OutOfPhase("");
        }

        LOG(INFO) << "ending " << player_id << "\'s turn";
        game_state->endTurn();
        // We put the next player into action phase
        return std::make_unique<shared::ActionPhaseOrder>();
    }

    GameInterface::response_t GameInterface::ChooseNCardsFromHandDecision_handler(
            std::unique_ptr<shared::ChooseNCardsFromHandDecision> /*action_decision*/,
            const Player::id_t & /*player_id*/)
    {
        // TODO: Implement for MVP 3
        LOG(ERROR) << "Not implemented yet, i will kill myself now:) much fun debugging!";
        throw std::runtime_error("Not implemented yet");
    }

    GameInterface::response_t GameInterface::PlayActionCardDecision_response_handler(
            std::unique_ptr<shared::PlayActionCardDecision> action_decision, const Player::id_t &player_id,
            const std::string &in_response_to)
    {
        // Checks for validity are done in the behaviour chain
        return (cur_behaviours->receiveAction(*game_state, player_id, std::move(action_decision), in_response_to))
                .value();
    }

    GameInterface::response_t
    GameInterface::BuyCardDecision_response_handler(std::unique_ptr<shared::BuyCardDecision> /*action_decision*/,
                                                    const Player::id_t & /*player_id*/,
                                                    const std::string & /*in_response_to*/)
    {
        // TODO: Implement for MVP 3
        LOG(ERROR) << "Not implemented";
        throw std::runtime_error("Not implemented");
    }

    GameInterface::response_t
    GameInterface::EndTurnDecision_response_handler(std::unique_ptr<shared::EndTurnDecision> /*action_decision*/,
                                                    const Player::id_t & /*player_id*/,
                                                    const std::string & /*in_response_to*/)
    {
        // TODO: Implement for MVP 3
        LOG(ERROR) << "Not implemented";
        throw std::runtime_error("Not implemented");
    }

    GameInterface::response_t GameInterface::ChooseNCardsFromHandDecision_response_handler(
            std::unique_ptr<shared::ChooseNCardsFromHandDecision> /*action_decision*/,
            const Player::id_t & /*player_id*/, const std::string & /*in_response_to*/)
    {
        // TODO: Implement for MVP 3
        LOG(ERROR) << "Not implemented";
        throw std::runtime_error("Not implemented");
    }

} // namespace server
