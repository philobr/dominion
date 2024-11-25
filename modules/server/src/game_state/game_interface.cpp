#include <server/game/game_state/game_interface.h>
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

    GameInterface::response_t GameInterface::receive_action(std::unique_ptr<shared::ActionDecision> action_decision,
                                                            const std::optional<std::string> &in_response_to,
                                                            const Player::id_t &affected_player_id)
    {
        if ( in_response_to.has_value() ) {
            LOG(ERROR) << "this is not implemented yet!";
            throw std::runtime_error("not implemented");
        }

        return handle_action(std::move(action_decision), affected_player_id);
        /* This might become useful later on
         * TODO: use this or delete it
        return in_response_to.has_value()
                ? handle_response(std::move(action_decision), affected_player_id, in_response_to.value());
                : handle_action(std::move(action_decision), affected_player_id)
        */
    }

    GameInterface::response_t GameInterface::handle_action(std::unique_ptr<shared::ActionDecision> action_decision,
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

    GameInterface::response_t
    GameInterface::handle_response(std::unique_ptr<shared::ActionDecision> /*action_decision*/,
                                   const std::string & /*in_response_to*/, const Player::id_t & /*affected_player_id*/)
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
    GameInterface::PlayActionCardDecision_handler(std::unique_ptr<shared::PlayActionCardDecision> /*action_decision*/,
                                                  const Player::id_t & /*player_id*/)
    {
        if (game_state->getPhase() != GamePhase::ACTION_PHASE) {
            LOG(WARN) << "player(" << player_id << ") is currently not in the action phase, retrying";
            return std::make_unique<shared::ActionPhaseOrder>();
        }
        
        game_state->try_play(player_id, action_decision->card);

        // TODO: Implement for MVP 3
        LOG(ERROR) << "Not implemented yet, i will kill myself now:) much fun debugging!";
        throw std::runtime_error("Not implemented yet");
    }

    GameInterface::response_t
    GameInterface::BuyCardDecision_handler(std::unique_ptr<shared::BuyCardDecision> action_decision,
                                           const Player::id_t &player_id)
    {
        if ( game_state->getPhase() != GamePhase::BUY_PHASE ) {
            LOG(WARN) << "player(" << player_id << ") is currently not in the buy phase, retrying";
            // TODO: this makes no sense, the player will just try to buy again
            return std::make_unique<shared::BuyPhaseOrder>();
        }

        game_state->try_buy(player_id, action_decision->card);

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
        game_state->end_turn();
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
            std::unique_ptr<shared::PlayActionCardDecision> /*action_decision*/, const Player::id_t & /*player_id*/,
            const std::string & /*in_response_to*/)
    {
        // TODO: Implement for MVP 3
        LOG(ERROR) << "Not implemented";
        throw std::runtime_error("Not implemented");
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
