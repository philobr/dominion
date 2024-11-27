
#include <memory>

#include <server/network/message_handler.h>
#include <shared/utils/assert.h>
#include <shared/utils/logger.h>

using namespace shared;

namespace server
{
    void MessageHandler::handleMessage(std::unique_ptr<shared::ClientToServerMessage> &message)
    {
        // Make sure that only one thread can access the message handler at a time
        // This is not very efficient, but it is the simplest way to make sure that
        // the message handler is thread-safe.
        std::lock_guard<std::mutex> lock(mutex_);

        auto &msg_ref = *message;

#define HANDLE(message_type, handler_func)                                                                             \
    if ( typeid(msg_ref) == typeid(message_type) ) {                                                                   \
        LOG(INFO) << "Trying to handle: " << #message_type;                                                            \
        std::unique_ptr<message_type> unique_##message_type(static_cast<(message_type) *>(message.release()));           \
        handler_func(std::move(unique_##message_type));                                                                \
        return;                                                                                                        \
    }

        // HANDLE(GameStateRequestMessage, this->lobby_manager_.createLobby);  TODO: not implemented yet
        HANDLE(CreateLobbyRequestMessage, this->lobby_manager_.createLobby);
        HANDLE(JoinLobbyRequestMessage, this->lobby_manager_.joinLobby);
        HANDLE(StartGameRequestMessage, this->lobby_manager_.startGame);

#undef HANDLE

        LOG(ERROR) << "Message Handler got unknown message";
        throw std::runtime_error("Unreachable code");
    }
} // namespace server
