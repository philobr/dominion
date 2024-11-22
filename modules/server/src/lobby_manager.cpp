#include <server/lobbies/lobby_manager.h>
#include <shared/utils/uuid_generator.h>


bool server::LobbyManager::lobby_exists(std::string lobby_id)
{
    return games.find(lobby_id) != games.end();
}

void server::LobbyManager::create_lobby(std::unique_ptr<shared::CreateLobbyRequestMessage> request)
{
    std::cerr << "Got in LobbyManager" << std::endl;
    std::string lobby_id = request->game_id;
    shared::PlayerBase::id_t game_master_id = request->player_id;
    // Lobby already exists
    if ( (games.size() > 0) && lobby_exists(lobby_id) ) {
        shared::ResultResponseMessage failure_message = shared::ResultResponseMessage(
                lobby_id, uuid_generator::generate_uuid_v4(), false, request->message_id, "Lobby already exists");
        message_interface->send_message(std::make_unique<shared::ResultResponseMessage>(failure_message),
                                        game_master_id);
        return;
    }

    games[lobby_id] = std::make_shared<Lobby>(game_master_id, lobby_id);

    std::vector<shared::CardBase::id_t> available_cards =
            std::vector<shared::CardBase::id_t>(); // TODO implement available cards
    shared::CreateLobbyResponseMessage create_lobby_message = shared::CreateLobbyResponseMessage(
            lobby_id, uuid_generator::generate_uuid_v4(), available_cards, request->message_id);
    message_interface->send_message(std::make_unique<shared::CreateLobbyResponseMessage>(create_lobby_message),
                                    game_master_id);
    std::cerr << "Got done with create lobby" << std::endl;
    return;
};

void server::LobbyManager::join_lobby(std::unique_ptr<shared::JoinLobbyRequestMessage> request)
{
    std::string lobby_id = request->game_id;
    shared::PlayerBase::id_t player_id = request->player_id;
    // Lobby does not exist
    if ( games.find(lobby_id) == games.end() ) {
        // TODO: Provide game_id and message_id
        shared::ResultResponseMessage failure_message = shared::ResultResponseMessage(
                lobby_id, uuid_generator::generate_uuid_v4(), false, request->message_id, "Lobby does not exist");
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
                lobby_id, uuid_generator::generate_uuid_v4(), false, request->message_id, "Lobby does not exist");
        message_interface->send_message(std::make_unique<shared::ResultResponseMessage>(failure_message), requestor_id);
        return;
    }

    games.at(lobby_id)->start_game(*message_interface, std::move(request));
};

void server::LobbyManager::receive_action(std::unique_ptr<shared::ActionDecisionMessage> msg)
{
    std::string lobby_id = msg->game_id;
    shared::PlayerBase::id_t player_id = msg->player_id;
    // Lobby does not exist
    if ( !lobby_exists(lobby_id) ) {
        shared::ResultResponseMessage failure_message = shared::ResultResponseMessage(
                lobby_id, uuid_generator::generate_uuid_v4(), false, msg->message_id, "Lobby does not exist");
        message_interface->send_message(std::make_unique<shared::ResultResponseMessage>(failure_message), player_id);
        return;
    }
    games.at(lobby_id)->receive_action(*message_interface, std::move(msg));
}