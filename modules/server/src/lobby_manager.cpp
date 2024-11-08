#include <server/lobbies.h>


server::LobbyManager::LobbyManager(MessageInterface &message_interface) :
    games(std::map<std::string, Lobby>()), message_interface(&message_interface) {};

void server::LobbyManager::create_lobby(shared::CreateLobbyRequestMessage request)
{
    std::string lobby_id = request.game_id;
    shared::PlayerBase::id_t game_master = request.player_id;
    // Lobby already exists
    if ( (games.size() > 0) && (games.find(lobby_id) != games.end()) ) {
        shared::ResultResponseMessage failure_message =
                shared::ResultResponseMessage(false, request.message_id, "Lobby already exists");
        message_interface->send_message(&failure_message, game_master);
        return;
    }
    Lobby new_lobby = Lobby(game_master);
    games.insert(std::pair<std::string, Lobby>(lobby_id, new_lobby));
    std::vector<shared::CardBase::id_t> available_cards =
            std::vector<shared::CardBase::id_t>(); // TODO implement available cards
    shared::CreateLobbyResponseMessage create_lobby_message =
            shared::CreateLobbyResponseMessage(available_cards, request.message_id);
    message_interface->send_message(&create_lobby_message, game_master);
    return;
};

void server::LobbyManager::join_lobby(shared::JoinLobbyRequestMessage request)
{
    std::string lobby_id = request.game_id;
    shared::PlayerBase::id_t player_id = request.player_id;
    // Lobby does not exist
    if ( games.find(lobby_id) == games.end() ) {
        shared::ResultResponseMessage failure_message =
                shared::ResultResponseMessage(false, request.message_id, "Lobby does not exist");
        message_interface->send_message(&failure_message, player_id);
        return;
    }
    games[lobby_id].join(*message_interface, request);
    return;
};

void server::LobbyManager::start_game(shared::StartGameRequestMessage request)
{
    std::string lobby_id = request.game_id;
    shared::PlayerBase::id_t requestor_id = request.player_id;
    // Lobby does not exist
    if ( games.find(lobby_id) == games.end() ) {
        shared::ResultResponseMessage failure_message =
                shared::ResultResponseMessage(false, request.message_id, "Lobby does not exist");
        message_interface->send_message(&failure_message, requestor_id);
        return;
    }
    games[lobby_id].start_game(*message_interface, request);
    return;
};