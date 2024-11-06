
#include <memory>

#include <server/message_handler.h>
#include <shared/message_types.h>
#include <shared/utils/assert.h>

using namespace shared;

namespace server
{
    void MessageHandler::HandleMessage(const std::string &message /* TODO: Also pass MessageInterface in here */)
    {
        std::unique_ptr<ClientToServerMessage> message_parsed;
        message_parsed = ClientToServerMessage::from_json(message);

        if ( message_parsed == nullptr ) {
            // TODO: Handle invalid message
        }

        if ( GameStateRequestMessage *gsrm = dynamic_cast<GameStateRequestMessage *>(message_parsed.get()) ) {
            (void)gsrm; // TODO: Handle game state request message
        } else if ( CreateLobbyRequestMessage *clrm =
                            dynamic_cast<CreateLobbyRequestMessage *>(message_parsed.get()) ) {
            (void)clrm; // TODO: Handle create lobby request message
        } else if ( JoinLobbyRequestMessage *jlrm = dynamic_cast<JoinLobbyRequestMessage *>(message_parsed.get()) ) {
            (void)jlrm; // TODO: Handle join lobby request message
        } else if ( StartGameRequestMessage *sgrm = dynamic_cast<StartGameRequestMessage *>(message_parsed.get()) ) {
            (void)sgrm; // TODO: Handle start game request message
        } else if ( ActionDecisionMessage *adm = dynamic_cast<ActionDecisionMessage *>(message_parsed.get()) ) {
            (void)adm; // TODO: Handle action decision message
        } else {
            // This code should never be reached
            ASSERT_FALSE(true, "Unknown message type");
        }
    }
} // namespace server
