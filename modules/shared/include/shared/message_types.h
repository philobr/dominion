#pragma once

#include <optional>
#include <string>
#include <vector>

#include "game_state.h"

namespace shared
{

    class Message
    {
    public:
        static void from_json(std::string json);
        virtual void to_json() = 0;

        std::string game_id;
        std::string message_id;
    };

    /* ======= client -> server ======= */

    class ClientToServerMessage : public Message
    {
    public:
        PlayerBase::id_t player_id;
    };

    class GameStateRequestMessage : public ClientToServerMessage
    {};

    class CreateLobbyRequestMessage : public ClientToServerMessage
    {};

    class JoinLobbyRequestMessage : public ClientToServerMessage
    {};

    class StartGameRequestMessage : public ClientToServerMessage
    {
    public:
        std::vector<CardBase::id_t> selected_cards;
    };

    class ActionDecisionMessage : public ClientToServerMessage
    {};

    /* ======= server -> client ======= */

    class ServerToClientMessage : public Message
    {};

    class GameStateResponseMessage : public ServerToClientMessage
    {
    public:
        std::optional<std::string> in_response_to;
        ReducedGameState game_state;
    };

    class CreateLobbyResponseMessage : public ServerToClientMessage
    {
    public:
        std::optional<std::string> in_response_to;
        std::vector<CardBase::id_t> available_cards;
    };

    class JoinLobbyBroadcastMessage : public ServerToClientMessage
    {
    public:
        JoinLobbyBroadcastMessage(shared::PlayerBase::id_t id) : player_id(id) {}
        shared::PlayerBase::id_t player_id;
    };

    class StartGameResponseMessage : public ServerToClientMessage
    {};

    class EndGameBroadcastMessage : public ServerToClientMessage
    {
    public:
        // TODO add player_scores
    };

    class ResultResponseMessage : public ServerToClientMessage
    {
    public:
        ResultResponseMessage(bool success, std::optional<std::string> in_response_to = std::nullopt, std::optional<std::string> additional_information = std::nullopt)
            : in_response_to(in_response_to), success(success), additional_information(additional_information)
        {}
        std::optional<std::string> in_response_to;
        bool success;
        std::optional<std::string> additional_information;
    };

    class ActionOrderMessage : public ServerToClientMessage
    {
    public:
        std::optional<std::string> description;
        // TODO add phase and params
    };
} // namespace shared
