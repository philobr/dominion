#include <server/lobbies.h>

void server::LobbyManager::create_lobby(std::unique_ptr<shared::CreateLobbyRequestMessage> request)
{
    std::string lobby_id = request->game_id;
    shared::PlayerBase::id_t game_master_id = request->player_id;
    // Lobby already exists
    if ( (games.size() > 0) && (games.find(lobby_id) != games.end()) ) {
        // TODO: Provide game_id and message_id
        shared::ResultResponseMessage failure_message = shared::ResultResponseMessage(
                "game_id", "message_id", false, request->message_id, "Lobby already exists");
        message_interface->send_message(std::make_unique<shared::ResultResponseMessage>(failure_message), game_master_id);
        return;
    }

    games[lobby_id] = std::make_unique<Lobby>(game_master_id);

    std::vector<shared::CardBase::id_t> available_cards =
            std::vector<shared::CardBase::id_t>(); // TODO implement available cards
    // TODO: Provide game_id and message_id
    shared::CreateLobbyResponseMessage create_lobby_message =
            shared::CreateLobbyResponseMessage("game_id", "message_id", available_cards, request->message_id);
    message_interface->send_message(std::make_unique<shared::CreateLobbyResponseMessage>(create_lobby_message),
                                    game_master_id);
};

void server::LobbyManager::join_lobby(std::unique_ptr<shared::JoinLobbyRequestMessage> request)
{
    std::string lobby_id = request->game_id;
    shared::PlayerBase::id_t player_id = request->player_id;
    // Lobby does not exist
    if ( games.find(lobby_id) == games.end() ) {
        // TODO: Provide game_id and message_id
        shared::ResultResponseMessage failure_message = shared::ResultResponseMessage(
                "game_id", "message_id", false, request->message_id, "Lobby does not exist");
        message_interface->send_message(std::make_unique<shared::ResultResponseMessage>(failure_message), player_id);
        return;
    }

    games.at(lobby_id)->join(*message_interface, std::move(request));
};

void server::LobbyManager::start_game(std::unique_ptr<shared::StartGameRequestMessage> request)
{
    std::string lobby_id = request->game_id;
    shared::PlayerBase::id_t requestor_id = request->player_id;
    // Lobby does not exist
    if ( games.find(lobby_id) == games.end() ) {
        shared::ResultResponseMessage failure_message = shared::ResultResponseMessage(
                "game_id", "message_id", false, request->message_id, "Lobby does not exist");
        message_interface->send_message(std::make_unique<shared::ResultResponseMessage>(failure_message), requestor_id);
        return;
    }

    games.at(lobby_id)->start_game(*message_interface, std::move(request));
};
