
#include <server/lobbies/lobby_manager.h>
#include "server/network/basic_network.h"

namespace server
{
    void LobbyManager::handleMessage(std::unique_ptr<shared::ClientToServerMessage> &message)
    {
        if ( message == nullptr ) {
            LOG(ERROR) << "Received message is null";
            throw std::runtime_error("unreachable code");
        }

        // handle create lobby
        if ( dynamic_cast<shared::CreateLobbyRequestMessage *>(message.get()) != nullptr ) {
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

            message_interface->send<shared::ResultResponseMessage>(player_id, lobby_id, false, message->message_id,
                                                                   "Lobby does not exist");
            return;
        }

        auto &lobby = games.at(lobby_id);
        try {
            lobby->handleMessage(*message_interface, message);
        } catch ( std::exception &e ) {
            // the lobby only throws if we can not recover
            LOG(ERROR) << "Lobby: \'" << lobby_id
                       << "\' had a fatal error while handling a message. Error: " << e.what()
                       << "\nShutting down the lobby now...";

            lobby->terminate(*message_interface);
            games.erase(games.find(lobby_id));
            return;
        }

        if ( lobby->isGameOver() ) {
            LOG(DEBUG) << "Game finished in lobby: \'" << lobby_id << "\'. Deleting the lobby.";
            games.erase(games.find(lobby_id));
        }
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

            message_interface->send<shared::ResultResponseMessage>(game_master_id, lobby_id, false, request->message_id,
                                                                   "Lobby already exists");
            return;
        }

        LOG(INFO) << "Creating lobby with ID: " << lobby_id;

        try {
            games.emplace(lobby_id, std::make_shared<Lobby>(game_master_id, lobby_id));
        } catch ( std::exception &e ) {
            LOG(ERROR) << "Error while creating a new lobby. ID: \'" << lobby_id << "\', game_master: \'"
                       << game_master_id << "\'";
            message_interface->send<shared::ResultResponseMessage>(game_master_id, lobby_id, false, request->message_id,
                                                                   "Failed to create lobby: \'" + lobby_id +
                                                                           "\'. Please try again.");
            return;
        }

        std::vector<shared::CardBase::id_t> available_cards =
                std::vector<shared::CardBase::id_t>(); // TODO implement available cards

        message_interface->send<shared::CreateLobbyResponseMessage>(game_master_id, lobby_id, available_cards,
                                                                    request->message_id);
    };

    void LobbyManager::removePlayer(std::string &lobby_id, player_id_t &player_id){
        // other messages get forwarded to the lobby
        if ( !lobbyExists(lobby_id) ) {
            LOG(WARN) << "Tried removing player: " << player_id << " from inexistent lobby: " << lobby_id;
            return;
        }
        // get the lobby that the player should be removed from
        auto &lobby = games.at(lobby_id);

        if( lobby->gameRunning() ){
            // Remove the player from the lobby
            lobby->removePlayer(player_id);

            // End the game for the remaining players and remove the game
            lobby->terminate(*message_interface);
            games.erase(games.find(lobby_id));
        } else{
            // if lobby is in login screen, just remove the player
            lobby->removePlayer(player_id);
            
            // if lobby is empty, remove it
            if(lobby->getPlayers().size() == 0){
                games.erase(games.find(lobby_id));
            }
        }
    }
} // namespace server
