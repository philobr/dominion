
#include <memory>

#include <server/message_interface.h>
#include <shared/message_types.h>

namespace server
{
    void MessageInterface::broadcast_message(std::unique_ptr<shared::ServerToClientMessage> message)
    {
        throw std::runtime_error("Not implemented");
    }

    void MessageInterface::send_message(std::unique_ptr<shared::ServerToClientMessage> message)
    {
        throw std::runtime_error("Not implemented");
    }

    void MessageInterface::handle_request(std::unique_ptr<shared::ClientToServerMessage> message)
    {
        throw std::runtime_error("Not implemented");
    }
} // namespace server
