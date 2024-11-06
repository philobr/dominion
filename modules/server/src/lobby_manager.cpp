#include <server/lobbies.h>


server::LobbyManager::LobbyManager(MessageInterface message_interface) : message_interface(message_interface), games(std::map<game_id, Lobby>()) {};

void server::LobbyManager::create_lobby(shared::CreateLobbyRequestMessage request){
    std::string lobby_id = request.game_id;
    shared::PlayerBase::id_t game_master = request.player_id;
    // Lobby already exists
    if (games.find(lobby_id) == games.end()){
        message_interface.send_message(shared::ResultResponseMessage(false, request.message_id, "Lobby already exists"), game_master);
        return;
    }
    Lobby new_lobby = Lobby(game_master);
    games.insert(std::pair<std::string, Lobby>(lobby_id, new_lobby));
    message_interface.send_message(shared::CreateLobbyResponseMessage(all_cards), game_master);
    return;
}; 

void server::LobbyManager::join_lobby(shared::JoinLobbyRequestMessage request){
    std::string lobby_id = request.game_id;
    shared::PlayerBase::id_t player_id = request.player_id;
    // Lobby does not exist
    if (games.find(lobby_id) == games.end()){
        message_interface.send_message(shared::ResultResponseMessage(false, request.message_id, "Lobby does not exist"), player_id);
        return;
    }
    games[lobby_id].join(message_interface, request);
    message_interface.send_message(shared::ResultResponseMessage(true, request.message_id), player_id);
    return;
};

void server::LobbyManager::start_game(shared::StartGameRequestMessage request){
    std::string lobby_id = request.get_lobby_id();
    shared::PlayerBase::id_t requestor_id = request.get_sender_id();
    // Lobby does not exist
    if (games.find(lobby_id) == games.end()){
        message_interface.send_message(shared::ResultResponseMessage(false, request.message_id, "Lobby does not exist"), requestor_id); 
        return;
    }
    games[lobby_id].start_game(message_interface, request);
    return;
};