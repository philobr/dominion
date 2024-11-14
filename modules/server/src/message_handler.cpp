
#include <memory>

#include <server/network/message_handler.h>
#include <shared/utils/assert.h>

using namespace shared;

namespace server
{
    void MessageHandler::HandleMessage(std::unique_ptr<shared::ClientToServerMessage> message)
    {
        // Make sure that only one thread can access the message handler at a time
        // This is not very efficient, but it is the simplest way to make sure that
        // the message handler is thread-safe.
        std::lock_guard<std::mutex> lock(mutex_);

        if ( GameStateRequestMessage *gsrm = dynamic_cast<GameStateRequestMessage *>(message.get()) ) {
            (void)gsrm; // TODO: Handle game state request message
        } else if ( CreateLobbyRequestMessage *clrm = dynamic_cast<CreateLobbyRequestMessage *>(message.get()) ) {
            std::unique_ptr<CreateLobbyRequestMessage> clrm_ptr(clrm);
            this->lobby_manager_.create_lobby(std::move(clrm_ptr));
        } else if ( JoinLobbyRequestMessage *jlrm = dynamic_cast<JoinLobbyRequestMessage *>(message.get()) ) {
            std::unique_ptr<JoinLobbyRequestMessage> jlrm_ptr(jlrm);
            this->lobby_manager_.join_lobby(std::move(jlrm_ptr));
        } else if ( StartGameRequestMessage *sgrm = dynamic_cast<StartGameRequestMessage *>(message.get()) ) {
            std::unique_ptr<StartGameRequestMessage> sgrm_ptr(sgrm);
            this->lobby_manager_.start_game(std::move(sgrm_ptr));
        } else if ( ActionDecisionMessage *adm = dynamic_cast<ActionDecisionMessage *>(message.get()) ) {
            std::unique_ptr<ActionDecisionMessage> adm_ptr(adm);
            this->lobby_manager_.receive_action(std::move(adm_ptr));
        } else {
            // This code should never be reached
            _ASSERT_FALSE(true, "Unknown message type");
        }
    }
} // namespace server
