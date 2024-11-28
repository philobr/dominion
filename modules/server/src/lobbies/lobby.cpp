
#include <server/lobbies/lobby.h>
#include <shared/game/game_state/board_base.h>
#include <shared/utils/assert.h>
#include <shared/utils/logger.h>

namespace server
{
    Lobby::Lobby(const Player::id_t &game_master, const std::string &lobby_id) :
        game_interface(nullptr), game_master(game_master), lobby_id(lobby_id)
    {
        LOG(INFO) << "Lobby constructor called with lobby_id: " << lobby_id;
        players.push_back(game_master);
    };

    void Lobby::handleMessage(MessageInterface &message_interface,
                              std::unique_ptr<shared::ClientToServerMessage> &message)
    {
        // NOLINTBEGIN(bugprone-macro-parentheses)
#define HANDLE(message_type, handler_func)                                                                             \
    if ( auto req = dynamic_cast<shared::message_type *>(message.get()) ) {                                            \
        LOG(INFO) << "Trying to handle: " << #message_type;                                                            \
        std::unique_ptr<shared::message_type> casted_message(static_cast<shared::message_type *>(message.release()));  \
        handler_func(message_interface, casted_message);                                                               \
        return;                                                                                                        \
    }
        // NOLINTEND(bugprone-macro-parentheses)

        // handle messages the lobby is responsible for
        HANDLE(JoinLobbyRequestMessage, addPlayer);
        HANDLE(StartGameRequestMessage, startGame);
        HANDLE(GameStateRequestMessage, getGameState);

        const auto &requestor_id = message->player_id;

        if ( !playerInLobby(requestor_id) ) {
            LOG(DEBUG) << "Received Action and Player is not in the requested lobby. Lobby ID: " << lobby_id
                       << " , Player ID: " << requestor_id << " , Message ID: " << message->message_id;
            message_interface.send<shared::ResultResponseMessage>(requestor_id, lobby_id, false, message->message_id,
                                                                  "Player is not in the lobby");
            return;
        }

        if ( !gameRunning() ) {
            LOG(ERROR) << "Tried to perform an action, but the game has not started yet";
            message_interface.send<shared::ResultResponseMessage>(requestor_id, lobby_id, false, message->message_id,
                                                                  "Game has not started yet!");
            throw std::runtime_error("unreachable code");
        }

        auto order_msg = std::make_unique<shared::ActionOrderMessage>(lobby_id, game_interface->handleMessage(message));
        message_interface.sendMessage(std::move(order_msg), requestor_id);
    }

    void Lobby::getGameState(MessageInterface &message_interface,
                             std::unique_ptr<shared::GameStateRequestMessage> &request)
    {
        LOG(ERROR) << "Not implemented yet";
        throw std::runtime_error("not implemented yet");

        const auto &player_id = request->player_id;
        if ( !gameRunning() ) {
            LOG(WARN) << "Tried to get the gamestate, but the game has not started yet";
            message_interface.send<shared::ResultResponseMessage>(player_id, lobby_id, false, request->message_id,
                                                                  "Game has already started");
            return; // we do nothing in this case
        }

        LOG(WARN) << "TODO: we always broadcast the gamestate if requested!";
        broadcastGameState(message_interface);
    }

    void Lobby::addPlayer(MessageInterface &message_interface,
                          std::unique_ptr<shared::JoinLobbyRequestMessage> &request)
    {
        const auto &requestor_id = request->player_id;
        LOG(INFO) << "Lobby::join called with Lobby ID: " << lobby_id << " and Player ID: " << requestor_id;

        if ( gameRunning() ) {
            LOG(WARN) << "Tried to join a game that has already started!";
            message_interface.send<shared::ResultResponseMessage>(requestor_id, lobby_id, false, request->message_id,
                                                                  "Game has already started");
            return; // we do nothing in this case
        }

        // Check if player is already in the lobby
        if ( playerInLobby(requestor_id) ) {
            LOG(DEBUG) << "Player is already in the lobby. Lobby ID: " << lobby_id << " , Player ID: " << requestor_id;
            message_interface.send<shared::ResultResponseMessage>(requestor_id, lobby_id, false, request->message_id,
                                                                  "Player is already in the lobby");
            return;
        }

        if ( players.size() >= shared::board_config::MAX_PLAYER_COUNT ) {
            LOG(DEBUG) << "Lobby is full. Lobby ID: " << lobby_id << " , Player ID: " << requestor_id
                       << " , Max players: " << shared::board_config::MAX_PLAYER_COUNT;
            message_interface.send<shared::ResultResponseMessage>(requestor_id, lobby_id, false, request->message_id,
                                                                  "Lobby is full");
            return;
        }

        // Add player to the lobby
        players.push_back(requestor_id);

        // Send JoinLobbyBroadcast to all players
        message_interface.broadcast<shared::JoinLobbyBroadcastMessage>(players, lobby_id, players);

        // TODO: do we need this? isnt receiving a joinlobbybroadcast message enough?
        message_interface.send<shared::ResultResponseMessage>(requestor_id, lobby_id, true, request->message_id);
    };

    // PRE: selected_cards are validated in message parsing
    void Lobby::startGame(MessageInterface &message_interface,
                          std::unique_ptr<shared::StartGameRequestMessage> &request)
    {
        const auto &requestor_id = request->player_id;
        LOG(INFO) << "Lobby::start_game called with Lobby ID: " << lobby_id << " and Player ID: " << requestor_id;

        if ( gameRunning() ) {
            LOG(WARN) << "Tried to start a game that has already started!";
            message_interface.send<shared::ResultResponseMessage>(requestor_id, lobby_id, false, request->message_id,
                                                                  "Game has already started");
            return; // we do nothing in this case
        }

        // Check if gamemaster is starting the game
        if ( requestor_id != game_master ) {
            LOG(DEBUG) << "Lobby::start_game is called by someone differnt than game master. Lobby ID: " << lobby_id
                       << " , Player ID: " << requestor_id << " , Game Master ID: " << game_master;
            message_interface.send<shared::ResultResponseMessage>(requestor_id, lobby_id, false, request->message_id,
                                                                  "Only the game master can start the game");
            return;
        }

        // check if the game has an allowed number of players
        if ( players.size() < shared::board_config::MIN_PLAYER_COUNT ||
             players.size() > shared::board_config::MAX_PLAYER_COUNT ) {
            LOG(DEBUG) << "Lobby::start_game is called with not enough or to many players. Lobby ID: " << lobby_id
                       << " , Player ID: " << requestor_id << " , Number of players: " << players.size();
            message_interface.send<shared::ResultResponseMessage>(requestor_id, lobby_id, false, request->message_id,
                                                                  "Invalid player count to start game");

            if ( players.size() > shared::board_config::MAX_PLAYER_COUNT ) {
                LOG(ERROR) << "we somehow have too many players in the lobby. (" << players.size() << " > "
                           << shared::board_config::MAX_PLAYER_COUNT << ")";
                throw std::runtime_error("invalid state, unreachable code");
            }
            return;
        }

        // Create new game interface
        game_interface = GameInterface::make(lobby_id, request->selected_cards, players);

        // send messages
        LOG(INFO) << "Sending StartGameBroadcastMessage in Lobby ID: " << lobby_id;
        message_interface.broadcast<shared::StartGameBroadcastMessage>(players, lobby_id);

        broadcastGameState(message_interface);
    }
} // namespace server
