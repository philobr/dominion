
#include <server/lobbies/lobby_manager.h>

namespace server
{
    void LobbyManager::handleMessage(std::unique_ptr<shared::ClientToServerMessage> &message)
    {
        if ( message == nullptr ) {
            LOG(ERROR) << "Received message is null";
            throw std::runtime_error("unreachable code");
        }

        // handle create lobby
        if ( auto *create_lobby_request = dynamic_cast<shared::CreateLobbyRequestMessage *>(message.get()) ) {
            LOG(INFO) << "Trying to handle: CreateLobbyRequestMessage";
            std::unique_ptr<shared::CreateLobbyRequestMessage> unique_CreateLobbyRequestMessage(
                    static_cast<shared::CreateLobbyRequestMessage *>(message.release()));

            createLobby(unique_CreateLobbyRequestMessage);
            return;
        }

        // other messages get forwarded to the lobby
        const auto &lobby_id = message->game_id;
        if ( !lobbyExists(message->game_id) ) {
            const auto &player_id = message->player_id;
            LOG(WARN) << "Tried to access a nonexistent LobbyID: " << lobby_id << ", by PlayerID: " << player_id;

            message_interface->send<shared::ResultResponseMessage>(player_id, lobby_id, false, message->message_id,
                                                                   "Lobby does not exist");
            return;
        }

        games.at(lobby_id)->handleMessage(*(message_interface), message);
    }

    void LobbyManager::createLobby(std::unique_ptr<shared::CreateLobbyRequestMessage> &request)
    {
        std::string lobby_id = request->game_id;
        Player::id_t game_master_id = request->player_id;
        LOG(INFO) << "LobbyManager::create_lobby called with Lobby ID: " << lobby_id
                  << " and Player ID: " << game_master_id;

        // Lobby already exists
        if ( lobbyExists(lobby_id) ) {
            LOG(DEBUG) << "Tried creating lobby that already exists. Game ID: " << lobby_id
                       << " , Player ID: " << game_master_id;

            message_interface->send<shared::ResultResponseMessage>(game_master_id, lobby_id, false, request->message_id,
                                                                   "Lobby already exists");
            return;
        }

        LOG(INFO) << "Creating lobby with ID: " << lobby_id;
        games.emplace(lobby_id, std::make_shared<Lobby>(game_master_id, lobby_id));

        std::vector<shared::CardBase::id_t> available_cards =
                std::vector<shared::CardBase::id_t>(); // TODO implement available cards

        message_interface->send<shared::CreateLobbyResponseMessage>(game_master_id, lobby_id, available_cards,
                                                                    request->message_id);
    };
} // namespace server
