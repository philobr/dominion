
#include <memory>

#include <server/network/message_handler.h>
#include <shared/utils/assert.h>
#include <shared/utils/logger.h>

using namespace shared;

namespace server
{
    void MessageHandler::HandleMessage(std::unique_ptr<shared::ClientToServerMessage> message)
    {
        // Make sure that only one thread can access the message handler at a time
        // This is not very efficient, but it is the simplest way to make sure that
        // the message handler is thread-safe.
        std::lock_guard<std::mutex> lock(mutex_);

        LOG(INFO) << "Message Handler called";

        shared::ClientToServerMessage &msgRef = *message;
        if ( typeid(msgRef) == typeid(GameStateRequestMessage) ) {
            LOG(INFO) << "Handler: Processed GameStateRequest";
        } else if ( typeid(msgRef) == typeid(CreateLobbyRequestMessage) ) {
            std::unique_ptr<CreateLobbyRequestMessage> clrm(
                    static_cast<CreateLobbyRequestMessage *>(message.release()));
            this->lobby_manager_.create_lobby(std::move(clrm));
            LOG(INFO) << "Handler: Processed CreateLobbyRequest";
        } else if ( typeid(msgRef) == typeid(JoinLobbyRequestMessage) ) {
            std::unique_ptr<JoinLobbyRequestMessage> jlrm(static_cast<JoinLobbyRequestMessage *>(message.release()));
            this->lobby_manager_.join_lobby(std::move(jlrm));
            LOG(INFO) << "Handler: Processed JoinLobbyRequest";
        } else if ( typeid(msgRef) == typeid(StartGameRequestMessage) ) {
            std::unique_ptr<StartGameRequestMessage> sgrm(static_cast<StartGameRequestMessage *>(message.release()));
            this->lobby_manager_.start_game(std::move(sgrm));
            LOG(INFO) << "Handler: Processed StartGameRequest";
        } else if ( typeid(msgRef) == typeid(ActionDecisionMessage) ) {
            std::unique_ptr<ActionDecisionMessage> adm(static_cast<ActionDecisionMessage *>(message.release()));
            this->lobby_manager_.receive_action(std::move(adm));
            LOG(INFO) << "Handler: Processed ActionDecisionMessage";
        } else {
            // This code should never be reached
            LOG(ERROR) << "Message Handler got unknown message";
            _ASSERT_FALSE(true, "Unknown message type");
        }
    }
} // namespace server
