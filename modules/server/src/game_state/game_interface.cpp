#include <server/game/game_state/game_interface.h>

namespace server
{
    GameInterface::ptr_t GameInterface::make(const std::string &game_id,
                                             const std::vector<shared::CardBase::id_t> &play_cards,
                                             const std::vector<Player::id_t> &player_ids)
    {
        return ptr_t(new GameInterface(game_id, play_cards, player_ids));
    }

    GameInterface::response_t GameInterface::receive_action(std::unique_ptr<shared::ActionDecision> action_decision,
                                                            const std::optional<std::string> &in_response_to,
                                                            const Player::id_t &affected_player_id)
    {
        return handle_action(std::move(action_decision), affected_player_id);
        /* This might become useful later on
         * TODO: use this or delete it
        return in_response_to.has_value()
                ? handle_action(std::move(action_decision), affected_player_id)
                : handle_response(std::move(action_decision), affected_player_id, in_response_to.value());
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

        throw std::runtime_error("Unreachable code");
    }

    GameInterface::response_t GameInterface::handle_response(std::unique_ptr<shared::ActionDecision> action_decision,
                                                             const std::string &in_response_to,
                                                             const Player::id_t &affected_player_id)
    {
#define HANDLE_RESPONSE(type)                                                                                          \
    if ( dynamic_cast<shared::type *>(action_decision.get()) ) {                                                       \
        return type##_response_handler(                                                                                \
                std::unique_ptr<shared::type>(static_cast<shared::type *>(action_decision.release())),                 \
                affected_player_id, in_response_to);                                                                   \
    }

        HANDLE_RESPONSE(PlayActionCardDecision);
        HANDLE_RESPONSE(BuyCardDecision);
        HANDLE_RESPONSE(EndTurnDecision);
        HANDLE_RESPONSE(ChooseNCardsFromHandDecision);

        throw std::runtime_error("Unreachable code");
    }

    GameInterface::response_t GameInterface::PlayActionCardDecision_handler(
            std::unique_ptr<shared::PlayActionCardDecision> action_decision,
            const Player::id_t &player_id) {
        // TODO: Implement for MVP 3
        throw std::runtime_error("Not implemented yet");
    }

    GameInterface::response_t GameInterface::BuyCardDecision_handler(
            std::unique_ptr<shared::BuyCardDecision> action_decision,
            const Player::id_t &player_id) {
        switch (phase) {
            case Phase::BUY_PHASE:
            case Phase::ACTION_PHASE:
                const bool success = game_state->try_buy(player_id, action_decision->card);
                if (!success) {
                    // TODO: Log error
                }
                return std::make_unique<shared::BuyPhaseOrder>();
            case Phase::PLAYING_ACTION_CARD:
                // TODO: Log error
                // TODO: Implement for MVP 3
                throw std::runtime_error("Not implemented yet");
            default:
                throw std::runtime_error("Unreachable code");
        }
    }

    GameInterface::response_t GameInterface::EndTurnDecision_handler(
            std::unique_ptr<shared::EndTurnDecision> action_decision,
            const Player::id_t &player_id) {
        switch (phase) {
            case Phase::BUY_PHASE:
            case Phase::ACTION_PHASE:
                // We end the turn of the current player
                game_state->end_turn(player_id);
                // We put the next player into action phase
                return std::make_unique<shared::ActionPhaseOrder>();
            case Phase::PLAYING_ACTION_CARD:
                // TODO: Log error
                // TODO: Implement for MVP 3
                throw std::runtime_error("Not implemented yet");
            default:
                throw std::runtime_error("Unreachable code");
        }
    }

    GameInterface::response_t GameInterface::ChooseNCardsFromHandDecision_handler(
            std::unique_ptr<shared::ChooseNCardsFromHandDecision> action_decision,
            const Player::id_t &player_id) {
        // TODO: Implement for MVP 3
        throw std::runtime_error("Not implemented yet");
    }

} // namespace server
