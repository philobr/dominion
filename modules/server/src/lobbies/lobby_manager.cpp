
#include <server/lobbies/lobby_manager.h>

namespace server
{
    bool LobbyManager::lobbyExists(std::string lobby_id) { return games.find(lobby_id) != games.end(); }

    void LobbyManager::handleMessage(std::unique_ptr<shared::ClientToServerMessage> &message)
    {
        if ( message == nullptr ) {
            LOG(ERROR) << "Received message is null";
            throw std::runtime_error("unreachable code");
        }

        // handle create lobby
        if ( typeid(message.get()) == typeid(shared::CreateLobbyRequestMessage) ) {
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

            message_interface->sendMessage(std::make_unique<shared::ResultResponseMessage>(
                                                   lobby_id, false, message->message_id, "Lobby does not exist"),
                                           player_id);
            return;
        }

        games.at(lobby_id)->handleMessage(*(message_interface.get()), message);
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
            shared::ResultResponseMessage failure_message =
                    shared::ResultResponseMessage(lobby_id, false, request->message_id, "Lobby already exists");
            message_interface->sendMessage(std::make_unique<shared::ResultResponseMessage>(failure_message),
                                           game_master_id);
            return;
        }

        LOG(INFO) << "Creating lobby with ID: " << lobby_id;
        games.emplace(lobby_id, std::make_shared<Lobby>(game_master_id, lobby_id));

        std::vector<shared::CardBase::id_t> available_cards =
                std::vector<shared::CardBase::id_t>(); // TODO implement available cards

        shared::CreateLobbyResponseMessage create_lobby_message =
                shared::CreateLobbyResponseMessage(lobby_id, available_cards, request->message_id);
        message_interface->sendMessage(std::make_unique<shared::CreateLobbyResponseMessage>(create_lobby_message),
                                       game_master_id);
    };
} // namespace server
