#pragma once

#include <memory>
#include <string>

#include <server/game/game_interface.h>
#include <server/game/game_state.h>
#include <server/network/message_interface.h>

#include <shared/message_types.h>

namespace server
{
    /**
     * @brief A lobby is a container for a game that is being played.
     *
     * A lobby is created by a game master, who is the first player to join the lobby.
     * The game master can start the game when they want to.
     */
    class Lobby
    {
    public:
        /**
         * @brief Create a new game lobby.
         * Game master is added to the players of the lobby.
         *
         * @param game_master The player who created the lobby.
         */
        Lobby(const Player::id_t &game_master,
              const std::string &lobby_id); // TODO: add message_interface shared_ptr here

        /**
         * @brief The lobby receives a generic message. It handles what it is responsible for and the rest gets passed
         * on to the game_interface
         *
         * @param message_interface
         * @param message
         */
        void handleMessage(MessageInterface &message_interface,
                           std::unique_ptr<shared::ClientToServerMessage>
                                   &message); // TODO: remove message_interface shared_ptr here

        /**
         * @brief Get the players in the lobby.
         *
         * THIS IS ONLY FOR TESTING. WOULD BE NICE TO REMOVE THIS
         *
         * @return A const reference vector of player ids.
         */
        const std::vector<Player::id_t> &getPlayers() const { return players; }

        /**
         * @brief Get the id of the game master.
         *
         * THIS IS ONLY FOR TESTING. WOULD BE NICE TO REMOVE THIS
         *
         * @return The id of the game master.
         */
        const Player::id_t &getGameMaster() const { return game_master; };

    private:
        std::unique_ptr<server::GameInterface> game_interface;
        Player::id_t game_master;

        std::vector<Player::id_t> players;
        std::string lobby_id;


        /**
         * @brief Adds a player to the lobby if the neccessary conditions are met.
         * The conditions beeing:
         * - game has not started yet
         * - game has < 4 players in it
         * - the player is not already in the lobby
         *
         * @param message_interface
         * @param request
         */
        void addPlayer(MessageInterface &message_interface, std::unique_ptr<shared::JoinLobbyRequestMessage> &request);

        /**
         * @brief Starts the game if the following is met:
         * - game has 2 <= num_players <= 4
         * - game is not already started
         * - player is the game_master
         *
         * @param message_interface
         * @param request
         */
        void startGame(MessageInterface &message_interface, std::unique_ptr<shared::StartGameRequestMessage> &request);

        /**
         * @brief Broadcasts the gamestate to all players (maybe change this in the future?) i kept it in as we will
         * change our messages and how they include the gamestate in the future.
         *
         * @param message_interface
         * @param request
         */
        void getGameState(MessageInterface &message_interface,
                          std::unique_ptr<shared::GameStateRequestMessage> &request);

        /**
         * @brief Check if a player is in the lobby.
         *
         * @param player_id The id of the player to check.
         *
         * @return True if the player is in the lobby, false otherwise.
         */
        inline bool playerInLobby(const Player::id_t &player_id)
        {
            return std::any_of(players.begin(), players.end(), [&](const auto &player) { return player == player_id; });
        }

        /**
         * @brief The game_interface gets initialised only when we start a game, so we can check if a game is already
         * running this way.
         *
         * @return true
         * @return false
         */
        inline bool gameRunning() const { return game_interface != nullptr; }

        /**
         * @brief Broadcasts the gamestate to all players in the lobby.
         *
         * @param message_interface
         */
        void broadcastGameState(MessageInterface &message_interface) const
        {
            std::for_each(players.begin(), players.end(),
                          [&](const auto &player_id)
                          {
                              LOG(INFO) << "Sending GameStateMessage in Lobby ID: " << lobby_id
                                        << " to Player ID: " << player_id;
                              message_interface.send<shared::GameStateMessage>(player_id, lobby_id,
                                                                               game_interface->getGameState(player_id));
                          });
        }
    };
} // namespace server