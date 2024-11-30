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
            game_state->setPhase(GamePhase::PLAYING_ACTION_CARD);
        } catch ( std::exception &e ) {
            LOG(ERROR) << "failed to play, TODO: handle this";
            throw std::runtime_error("failed to play card (GameInterface::PlayActionCardDecision_handler), this needs "
                                     "to be handled better");
        }

        cur_behaviours->loadBehaviours(action_decision->card_id);
        auto response = cur_behaviours->receiveAction(*game_state, player_id, std::nullopt, std::nullopt);
        if ( response.has_value() ) {
            // this means we will receive a response for this
            return std::move(response.value());
        }

        // the code below can probably be extracted into a seperate function
        return finishedPlayingCard();
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

        game_state->maybeSwitchPhase();
        switch ( game_state->getPhase() ) {
            case server::GamePhase::BUY_PHASE:
                return std::make_unique<shared::BuyPhaseOrder>();
            case server::GamePhase::ACTION_PHASE:
                return std::make_unique<shared::ActionPhaseOrder>();
            default:
                {
                    LOG(ERROR) << "invalid phase after buying a card";
                    throw std::runtime_error(
                            "invalid phase after buying a card in GameInterface::BuyCardDecision_handler");
                }
        }
    }

    GameInterface::response_t
    GameInterface::EndTurnDecision_handler(std::unique_ptr<shared::EndTurnDecision> action_decision,
                                           const Player::id_t &player_id)
    {
        if ( game_state->getPhase() == GamePhase::PLAYING_ACTION_CARD ) {
            LOG(ERROR) << "Player is trying to end his turn while playing a card";
            throw exception::OutOfPhase("");
        }

        LOG(INFO) << "ending " << player_id << "\'s turn";
        game_state->endTurn();

        return std::make_unique<shared::ActionPhaseOrder>();
    }

    GameInterface::response_t GameInterface::ChooseNCardsFromHandDecision_handler(
            std::unique_ptr<shared::ChooseNCardsFromHandDecision> action_decision, const Player::id_t &player_id)
    {
        auto order_msg =
                cur_behaviours->receiveAction(*game_state, player_id, std::move(action_decision), std::nullopt);

        if ( order_msg.has_value() ) {
            return std::move(order_msg.value());
        }

        // this will probably lead to errors, some behaviours need multiple responses so we need to handle this here but
        // i really dont care rn

        // this can be fixed by adding a isDone() function to the behaviours

        // finished playing the card
        return finishedPlayingCard();
    }

} // namespace server
