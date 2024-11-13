
#include <server/lobbies.h>
#include <shared/game/game_state/reduced_game_state.h>
#include <shared/utils/assert.h>

const unsigned int MAX_PLAYERS = 4;

server::Lobby::Lobby(shared::PlayerBase::id_t game_master) : game_master(game_master)
{
    players.push_back(game_master);
};

void server::Lobby::join(MessageInterface &message_interface, shared::JoinLobbyRequestMessage request)
{
    const shared::PlayerBase::id_t requestor_id = request.player_id;

    const bool player_already_in_lobby = std::any_of(players.begin(), players.end(),
                                              [&](const auto &player_id) { return player_id == requestor_id; });

    if ( player_already_in_lobby ) {
        shared::ResultResponseMessage failure_message =
                // TODO: provide game_id and message_id
                shared::ResultResponseMessage("game_id", "message_id", false, request.message_id,
                                              "Player is already in the lobby");
        message_interface.send_message(&failure_message, requestor_id);
        return;
    }

    if ( players.size() >= MAX_PLAYERS ) {
        shared::ResultResponseMessage failure_message =
                // TODO: provide game_id and message_id
                shared::ResultResponseMessage("game_id", "message_id", false, request.message_id, "Lobby is full");
        message_interface.send_message(&failure_message, requestor_id);
        return;
    }

    // Send JoinLobbyBroadcast to all players
    for ( const auto &player_id : players ) {
        // TODO: provide game_id and message_id
        shared::JoinLobbyBroadcastMessage join_message =
                shared::JoinLobbyBroadcastMessage("game_id", "message_id", requestor_id);
        message_interface.send_message(&join_message, player_id);
    }

    // Add player to the lobby
    players.push_back(requestor_id);

    // TODO: provide game_id and message_id
    shared::ResultResponseMessage success_message =
            shared::ResultResponseMessage("game_id", "message_id", true, request.message_id);

    message_interface.send_message(&success_message, requestor_id);
    return;
};

// PRE: selected_cards are validated in message parsing
void server::Lobby::start_game(MessageInterface &message_interface, shared::StartGameRequestMessage request)
{
    // Check if gamemaster is starting the game
    shared::PlayerBase::id_t requestor_id = request.player_id;
    if ( requestor_id != game_master ) {
        shared::ResultResponseMessage failure_message =
                // TODO: provide game_id and message_id
                shared::ResultResponseMessage("game_id", "message_id", false, request.message_id,
                                              "Only the game master can start the game");
        message_interface.send_message(&failure_message, requestor_id);
        return;
    }

    game_state = std::make_unique<GameState>(request.selected_cards, players);
    // set board cards
    // send game state to all players
    for ( const auto &player_id : players ) {
        // TODO: provide game_id and message_id
        shared::StartGameBroadcastMessage start_message = shared::StartGameBroadcastMessage("game_id", "message_id");
        message_interface.send_message(&start_message, player_id);
        // TODO: reenable this
        // shared::ReducedGameState reduced_game_state = game_state.get_reduced_state(player_id);
        // TODO: provide game_id and message_id
        shared::GameStateMessage game_state_message =
                shared::GameStateMessage("game_id", "message_id" /*, reduced_game_state */);
        message_interface.send_message(&game_state_message, player_id);
    }
    return;
};
