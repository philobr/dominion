
#include <server/lobbies/lobby.h>
#include <shared/utils/assert.h>
#include <shared/utils/uuid_generator.h>
#include <shared/utils/logger.h>

const unsigned int MAX_PLAYERS = 4;
namespace server
{
    Lobby::Lobby(shared::PlayerBase::id_t game_master, std::string lobby_id) :
        game_master(game_master), lobby_id(lobby_id)
    {
        players.push_back(game_master);
    };

    void Lobby::join(MessageInterface &message_interface, std::unique_ptr<shared::JoinLobbyRequestMessage> request)
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

        // Send JoinLobbyBroadcast to all players
        for ( const auto &player_id : players ) {
            shared::JoinLobbyBroadcastMessage join_message =
                    shared::JoinLobbyBroadcastMessage(lobby_id, uuid_generator::generate_uuid_v4(), requestor_id);
            message_interface.send_message(std::make_unique<shared::JoinLobbyBroadcastMessage>(join_message), player_id);
        }

        // Add player to the lobby
        players.push_back(requestor_id);

        shared::ResultResponseMessage success_message =
                shared::ResultResponseMessage(lobby_id, uuid_generator::generate_uuid_v4(), true, request->message_id);

        message_interface.send_message(std::make_unique<shared::ResultResponseMessage>(success_message), requestor_id);
        return;
    };

    // PRE: selected_cards are validated in message parsing
    void Lobby::start_game(MessageInterface &message_interface,
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

        // Create new game interface
        game_interface = GameInterface::make(lobby_id, request->selected_cards, players);

        // get game state for game_state_message later
        std::shared_ptr<GameState> game_state = game_interface->get_game_state();
        if(!game_state){
            LOG(ERROR) << "Game state is nullptr";
            throw std::runtime_error("Game state is nullptr");
        }

        // send messages
        for ( const auto &player_id : players ) {
            // send start game broadcast to all players
            shared::StartGameBroadcastMessage start_message =
                    shared::StartGameBroadcastMessage(lobby_id, uuid_generator::generate_uuid_v4());
            message_interface.send_message(std::make_unique<shared::StartGameBroadcastMessage>(start_message), player_id);

            // send game state to all players
            shared::ReducedGameState reduced_game_state = game_state->get_reduced_state(player_id);
            shared::GameStateMessage game_state_message =
                    shared::GameStateMessage(lobby_id, uuid_generator::generate_uuid_v4() /*, reduced_game_state */);
            message_interface.send_message(std::make_unique<shared::GameStateMessage>(game_state_message), player_id);
        }
    }
} // namespace server