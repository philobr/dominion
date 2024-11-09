#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "action_decision.h"
#include "game_state.h"

namespace shared
{

    class Message
    {
    public:
        virtual std::string to_json() = 0;

        std::string game_id;
        std::string message_id;

    protected:
        Message(std::string game_id, std::string message_id) : game_id(game_id), message_id(message_id) {}
        bool operator==(const Message &other) const;
    };

    /* ======= client -> server ======= */

    class ClientToServerMessage : public Message
    {
    public:
        static std::unique_ptr<ClientToServerMessage> from_json(const std::string &json);

        PlayerBase::id_t player_id;

    protected:
        ClientToServerMessage(std::string game_id, std::string message_id, PlayerBase::id_t player_id) :
            Message(game_id, message_id), player_id(player_id)
        {}
        bool operator==(const ClientToServerMessage &other) const;
    };

    class GameStateRequestMessage : public ClientToServerMessage
    {
    public:
        GameStateRequestMessage(std::string game_id, std::string message_id, PlayerBase::id_t player_id) :
            ClientToServerMessage(game_id, message_id, player_id)
        {}
        std::string to_json() override;
        bool operator==(const GameStateRequestMessage &other) const;
    };

    class CreateLobbyRequestMessage : public ClientToServerMessage
    {
    public:
        CreateLobbyRequestMessage(std::string game_id, std::string message_id, PlayerBase::id_t player_id) :
            ClientToServerMessage(game_id, message_id, player_id)
        {}
        std::string to_json() override;
        bool operator==(const CreateLobbyRequestMessage &other) const;
    };

    class JoinLobbyRequestMessage : public ClientToServerMessage
    {
    public:
        JoinLobbyRequestMessage(std::string game_id, std::string message_id, PlayerBase::id_t player_id) :
            ClientToServerMessage(game_id, message_id, player_id)
        {}
        std::string to_json() override;
        bool operator==(const JoinLobbyRequestMessage &other) const;
    };

    class StartGameRequestMessage : public ClientToServerMessage
    {
    public:
        /**
         * @param selected_cards The 10 cards selected by the game master to play with.
         * The size of the vector must be 10. Otherwise, an assertion will fail.
         */
        StartGameRequestMessage(std::string game_id, std::string message_id, PlayerBase::id_t player_id,
                                std::vector<CardBase::id_t> selected_cards);
        std::string to_json() override;
        bool operator==(const StartGameRequestMessage &other) const;

        std::vector<CardBase::id_t> selected_cards;
    };

    class ActionDecisionMessage : public ClientToServerMessage
    {
    public:
        ActionDecisionMessage(std::string game_id, std::string message_id, PlayerBase::id_t player_id,
                              std::unique_ptr<ActionDecision> decision,
                              std::optional<std::string> in_response_to = std::nullopt) :
            ClientToServerMessage(game_id, message_id, player_id), decision(std::move(decision)),
            in_response_to(in_response_to)
        {}
        std::string to_json() override;
        bool operator==(const ActionDecisionMessage &other) const;

        std::unique_ptr<ActionDecision> decision;
        std::optional<std::string> in_response_to;
    };

    /* ======= server -> client ======= */

    class ServerToClientMessage : public Message
    {
    public:
        /**
         * Parse a JSON string representing the message.
         *
         * Returns nullptr if the JSON is invalid.
         */
        static std::unique_ptr<ServerToClientMessage> from_json(const std::string &json);

    protected:
        ServerToClientMessage(std::string game_id, std::string message_id) : Message(game_id, message_id) {}
        bool operator==(const ServerToClientMessage &other) const;
    };

    class GameStateMessage : public ServerToClientMessage
    {
    public:
        GameStateMessage(std::string game_id, std::string message_id, /* ReducedGameState game_state, */
                         std::optional<std::string> in_response_to = std::nullopt) :
            ServerToClientMessage(game_id, message_id), /* game_state(game_state), */ in_response_to(in_response_to)
        {}
        std::string to_json() override;
        bool operator==(const GameStateMessage &other) const;

        // TODO: Add this back
        // ReducedGameState game_state;
        std::optional<std::string> in_response_to;
    };

    class CreateLobbyResponseMessage : public ServerToClientMessage
    {
    public:
        CreateLobbyResponseMessage(std::string game_id, std::string message_id,
                                   std::vector<CardBase::id_t> available_cards,
                                   std::optional<std::string> in_response_to = std::nullopt) :
            ServerToClientMessage(game_id, message_id), available_cards(available_cards), in_response_to(in_response_to)
        {}
        std::string to_json() override;
        bool operator==(const CreateLobbyResponseMessage &other) const;

        std::vector<CardBase::id_t> available_cards;
        std::optional<std::string> in_response_to;
    };

    class JoinLobbyBroadcastMessage : public ServerToClientMessage
    {
    public:
        JoinLobbyBroadcastMessage(std::string game_id, std::string message_id, PlayerBase::id_t player_id) :
            ServerToClientMessage(game_id, message_id), player_id(player_id)
        {}
        std::string to_json() override;
        bool operator==(const JoinLobbyBroadcastMessage &other) const;
        PlayerBase::id_t player_id;
    };

    class StartGameBroadcastMessage : public ServerToClientMessage
    {
    public:
        StartGameBroadcastMessage(std::string game_id, std::string message_id) :
            ServerToClientMessage(game_id, message_id)
        {}
        std::string to_json() override;
        bool operator==(const StartGameBroadcastMessage &other) const;
    };

    class EndGameBroadcastMessage : public ServerToClientMessage
    {
    public:
        EndGameBroadcastMessage(std::string game_id, std::string message_id) :
            ServerToClientMessage(game_id, message_id)
        {}
        std::string to_json() override;
        bool operator==(const EndGameBroadcastMessage &other) const;
        // TODO add player_scores
    };

    class ResultResponseMessage : public ServerToClientMessage
    {
    public:
        ResultResponseMessage(std::string game_id, std::string message_id, bool success,
                              std::optional<std::string> in_response_to = std::nullopt,
                              std::optional<std::string> additional_information = std::nullopt) :
            ServerToClientMessage(game_id, message_id), success(success), in_response_to(in_response_to),
            additional_information(additional_information)
        {}
        std::string to_json() override;
        bool operator==(const ResultResponseMessage &other) const;

        bool success;
        std::optional<std::string> in_response_to;
        std::optional<std::string> additional_information;
    };

    class ActionOrderMessage : public ServerToClientMessage
    {
    public:
        ActionOrderMessage(std::string game_id, std::string message_id,
                           std::optional<std::string> description = std::nullopt) :
            ServerToClientMessage(game_id, message_id), description(description)
        {}
        std::string to_json() override;
        bool operator==(const ActionOrderMessage &other) const;

        // TODO add phase and params
        std::optional<std::string> description;
    };
} // namespace shared
