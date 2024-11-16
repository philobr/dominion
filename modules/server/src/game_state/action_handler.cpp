#include <server/game/game_state/game_state.h>

namespace server
{
    void GameState::receive_action(std::unique_ptr<shared::ActionDecisionMessage> message,
                                   MessageInterface &message_interface)
    {
#define HANDLE_ACTION(type, handler_func)                                                                              \
    if ( auto _ = dynamic_cast<type *>(message->decision.get()) ) {                                                    \
        handler_func(std::move(message), message_interface);                                                           \
        return;                                                                                                        \
    }

        HANDLE_ACTION(shared::PlayActionCardDecision, PlayAction_handler);
        HANDLE_ACTION(shared::BuyCardDecision, BuyAction_handler);
        HANDLE_ACTION(shared::EndTurnDecision, EndTurn_handler);
        HANDLE_ACTION(shared::ChooseNCardsFromHandDecision, ChooseCards_handler);

        throw std::runtime_error("Unknown action type received");
    }

#define HEADER(message, type)                                                                                          \
    const auto &affected_player_id = message->player_id;                                                               \
    auto action = std::make_unique<type>(message->decision.release());

    void GameState::PlayAction_handler(std::unique_ptr<shared::ActionDecisionMessage> message,
                                       MessageInterface &message_interface)
    {
        HEADER(message, shared::PlayActionCardDecision);
    }

    void GameState::BuyAction_handler(std::unique_ptr<shared::ActionDecisionMessage> message,
                                      MessageInterface &message_interface)
    {
        HEADER(message, shared::BuyCardDecision);

        const auto &card_id_to_buy = action->card;
        if ( !try_buy(affected_player_id, card_id_to_buy) ) {
            // would be nice to get more details here...
            // send error msg -> failed
        }

        // send success msg
    }

    void GameState::EndTurn_handler(std::unique_ptr<shared::ActionDecisionMessage> message,
                                    MessageInterface &message_interface)
    {
        HEADER(message, shared::EndTurnDecision);

        if ( affected_player_id != get_current_player_id() ) {
            // return error msg -> not your turn
        }

        end_turn();

        // send response
    }

    void GameState::ChooseCards_handler(std::unique_ptr<shared::ActionDecisionMessage> message,
                                        MessageInterface &message_interface)
    {
        HEADER(message, shared::ChooseNCardsFromHandDecision);

        /**
         * assumption:
         * 1. the idea is, that this message is always received as a response to some player decision.
         *      -> meaning if in_response_to is not set we return an error
         *      -> if in_response_to is set, we can look up the last behaviour and and proceed (trash, discard,
         * whatever)
         *
         * issue:
         * this is basically just a specialistaion for militia and chapel (maybe 1-2 cards more)
         *
         * fix:
         * refactor to provide information about where we choose from (e.g. hand, discard, draw, played, trash) and what
         * we want to do with them.
         */
    }
} // namespace server