#pragma once

#include <map>
#include <memory>
#include <string>

#include <shared/game_state.h>
#include <shared/message_types.h>

#include <server/game_state.h>
#include <server/message_interface.h>

namespace server
{

    class Lobby
    {
    public:
        Lobby(shared::PlayerBase::id_t game_master);
        void join(MessageInterface message_interface,shared::JoinLobbyRequestMessage request);
        void start_game(MessageInterface message_interface, shared::StartGameRequestMessage request);
        void receive_action(std::unique_ptr<shared::ActionDecisionMessage> action, MessageInterface &message_interface);
        shared::ReducedGameState get_game_state(shared::PlayerBase::id_t player);

    private:
        GameState game_state;
        shared::PlayerBase::id_t game_master;
    };

    class LobbyManager
    {
    public:
        LobbyManager(MessageInterface message_interface);
        void create_lobby(shared::CreateLobbyRequestMessage request);
        void join_lobby(shared::JoinLobbyRequestMessage request);
        void start_game(shared::StartGameRequestMessage request);
        void receive_action(std::unique_ptr<shared::ActionDecisionMessage> action, MessageInterface &message_interface);
        shared::ReducedGameState get_game_state(std::string game_id, shared::PlayerBase::id_t player);

    private:
        std::map<std::string, Lobby> games;
        MessageInterface message_interface;
    };
} // namespace server
