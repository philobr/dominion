
#include <server/lobbies/lobby.h>
#include <shared/utils/assert.h>
#include <shared/utils/uuid_generator.h>

const unsigned int MAX_PLAYERS = 4;

server::Lobby::Lobby(shared::PlayerBase::id_t game_master, std::string lobby_id) :
    game_master(game_master), lobby_id(lobby_id)
{
    players.push_back(game_master);
};

void server::Lobby::join(MessageInterface &message_interface, std::unique_ptr<shared::JoinLobbyRequestMessage> request)
{
    const shared::PlayerBase::id_t requestor_id = request->player_id;

    const bool player_already_in_lobby = std::any_of(players.begin(), players.end(),
                                                     [&](const auto &player_id) { return player_id == requestor_id; });

    if ( player_already_in_lobby ) {
        shared::ResultResponseMessage failure_message =
                shared::ResultResponseMessage(lobby_id, uuid_generator::generate_uuid_v4(), false, request->message_id,
                                              "Player is already in the lobby");

        message_interface.send_message(std::make_unique<shared::ResultResponseMessage>(failure_message), requestor_id);
        return;
    }

    if ( players.size() >= MAX_PLAYERS ) {
        shared::ResultResponseMessage failure_message = shared::ResultResponseMessage(
                lobby_id, uuid_generator::generate_uuid_v4(), false, request->message_id, "Lobby is full");
        message_interface.send_message(std::make_unique<shared::ResultResponseMessage>(failure_message), requestor_id);
        return;
    }

    // Add player to the lobby
    players.push_back(requestor_id);

    std::cerr << players.size() << std::endl;
    // Send JoinLobbyBroadcast to all players
    for ( const auto &player_id : players ) {
        shared::JoinLobbyBroadcastMessage join_message =
                shared::JoinLobbyBroadcastMessage(lobby_id, uuid_generator::generate_uuid_v4(), players);
        message_interface.send_message(std::make_unique<shared::JoinLobbyBroadcastMessage>(join_message), player_id);
    }

    shared::ResultResponseMessage success_message =
            shared::ResultResponseMessage(lobby_id, uuid_generator::generate_uuid_v4(), true, request->message_id);

    message_interface.send_message(std::make_unique<shared::ResultResponseMessage>(success_message), requestor_id);
    return;
};

// PRE: selected_cards are validated in message parsing
void server::Lobby::start_game(MessageInterface &message_interface,
                               std::unique_ptr<shared::StartGameRequestMessage> request)
{
    // Check if gamemaster is starting the game
    shared::PlayerBase::id_t requestor_id = request->player_id;
    if ( requestor_id != game_master ) {
        shared::ResultResponseMessage failure_message =
                shared::ResultResponseMessage(lobby_id, uuid_generator::generate_uuid_v4(), false, request->message_id,
                                              "Only the game master can start the game");
        message_interface.send_message(std::make_unique<shared::ResultResponseMessage>(failure_message), requestor_id);
        return;
    }

    game_state = std::make_unique<GameState>(request->selected_cards, players);
    // set board cards
    // send game state to all players
    for ( const auto &player_id : players ) {
        shared::StartGameBroadcastMessage start_message =
                shared::StartGameBroadcastMessage(lobby_id, uuid_generator::generate_uuid_v4());
        message_interface.send_message(std::make_unique<shared::StartGameBroadcastMessage>(start_message), player_id);
        // TODO: reenable this
        // shared::ReducedGameState reduced_game_state = game_state.get_reduced_state(player_id);
        shared::GameStateMessage game_state_message =
                shared::GameStateMessage(lobby_id, uuid_generator::generate_uuid_v4() /*, reduced_game_state */);
        message_interface.send_message(std::make_unique<shared::GameStateMessage>(game_state_message), player_id);
    }
}
