#include <server/lobbies/lobby_manager.h>


bool server::LobbyManager::lobby_exists(std::string lobby_id) { return games.find(lobby_id) != games.end(); }

void server::LobbyManager::create_lobby(std::unique_ptr<shared::CreateLobbyRequestMessage> request)
{
    std::string lobby_id = request->game_id;
    Player::id_t game_master_id = request->player_id;
    LOG(INFO) << "LobbyManager::create_lobby called with Lobby ID: " << lobby_id << " and Player ID: " << game_master_id;
    // Lobby already exists
    if ( (games.size() > 0) && lobby_exists(lobby_id) ) {
        LOG(DEBUG) << "Tried creating lobby that already exists. Game ID: " << lobby_id << " , Player ID: " << game_master_id;
        shared::ResultResponseMessage failure_message =
                shared::ResultResponseMessage(lobby_id, false, request->message_id, "Lobby already exists");
        message_interface->send_message(std::make_unique<shared::ResultResponseMessage>(failure_message),
                                        game_master_id);
        return;
    }

    LOG(DEBUG) << "Creating lobby with ID: " << lobby_id;
    games[lobby_id] = std::make_shared<Lobby>(game_master_id, lobby_id);

    std::vector<shared::CardBase::id_t> available_cards =
            std::vector<shared::CardBase::id_t>(); // TODO implement available cards
    shared::CreateLobbyResponseMessage create_lobby_message =
            shared::CreateLobbyResponseMessage(lobby_id, available_cards, request->message_id);
    message_interface->send_message(std::make_unique<shared::CreateLobbyResponseMessage>(create_lobby_message),
                                    game_master_id);
    return;
};

void server::LobbyManager::join_lobby(std::unique_ptr<shared::JoinLobbyRequestMessage> request)
{
    std::string lobby_id = request->game_id;
    Player::id_t player_id = request->player_id;
    LOG(INFO) << "LobbyManager::join_lobby called with Lobby ID: " << lobby_id << " and Player ID: " << player_id;
    // Lobby does not exist
    if ( games.find(lobby_id) == games.end() ) {
        LOG(DEBUG) << "Tried joining lobby that does not exist. Game ID: " << lobby_id << " , Player ID: " << player_id;
        // TODO: Provide game_id and message_id
        shared::ResultResponseMessage failure_message =
                shared::ResultResponseMessage(lobby_id, false, request->message_id, "Lobby does not exist");
        message_interface->send_message(std::make_unique<shared::ResultResponseMessage>(failure_message), player_id);
        return;
    }

    games.at(lobby_id)->join(*message_interface, std::move(request));
};

void server::LobbyManager::start_game(std::unique_ptr<shared::StartGameRequestMessage> request)
{
    std::string lobby_id = request->game_id;
    Player::id_t player_id = request->player_id;
    LOG(INFO) << "LobbyManager::start_game called with Lobby ID: " << lobby_id << " and Player ID: " << player_id;
    // Lobby does not exist
    if ( games.find(lobby_id) == games.end() ) {
        LOG(DEBUG) << "Tried starting game in lobby that does not exist. Game ID: " << lobby_id << " , Player ID: " << player_id;
        shared::ResultResponseMessage failure_message =
                shared::ResultResponseMessage(lobby_id, false, request->message_id, "Lobby does not exist");
        message_interface->send_message(std::make_unique<shared::ResultResponseMessage>(failure_message), player_id);
        return;
    }

    games.at(lobby_id)->start_game(*message_interface, std::move(request));
};

void server::LobbyManager::receive_action(std::unique_ptr<shared::ActionDecisionMessage> msg)
{
    LOG(INFO) << "LobbyManager::receive_action called with Lobby ID: " << msg->game_id << " and Player ID: " << msg->player_id;
    std::string lobby_id = msg->game_id;
    Player::id_t player_id = msg->player_id;
    // Lobby does not exist
    if ( !lobby_exists(lobby_id) ) {
        LOG(DEBUG) << "Tried receiving action in lobby that does not exist. Game ID: " << lobby_id << " , Player ID: " << player_id;
        shared::ResultResponseMessage failure_message =
                shared::ResultResponseMessage(lobby_id, false, msg->message_id, "Lobby does not exist");
        message_interface->send_message(std::make_unique<shared::ResultResponseMessage>(failure_message), player_id);
        return;
    }
    games.at(lobby_id)->receive_action(*message_interface, std::move(msg));
}