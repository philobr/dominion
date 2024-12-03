
#include <memory>

#include <rapidjson/document.h>

#include <shared/game/game_state/reduced_game_state.h>
#include <shared/message_types.h>
#include <shared/utils/assert.h>
#include <shared/utils/json.h>

using namespace shared;
using namespace rapidjson;

/* ======= SERVER TO CLIENT MESSAGES ======= */

static std::unique_ptr<GameStateMessage> parseGameStateMessage(const Document &json, const std::string &game_id,
                                                               const std::string &message_id)
{
    std::optional<std::string> in_response_to;

    if ( !json.HasMember("game_state") ) {
        LOG(WARN) << "GameStateMessage: No game_state member";
        return nullptr;
    }
    std::unique_ptr<reduced::GameState> game_state = reduced::GameState::fromJson(json["game_state"]);
    if ( game_state == nullptr ) {
        LOG(WARN) << "GameStateMessage: Could not parse game_state";
        return nullptr;
    }

    GET_OPTIONAL_STRING_MEMBER(in_response_to, json, "in_response_to");

    return std::make_unique<GameStateMessage>(game_id, std::move(game_state), in_response_to, message_id);
}

static std::unique_ptr<CreateLobbyResponseMessage>
parseCreateLobbyResponse(const Document &json, const std::string &game_id, const std::string &message_id)
{
    std::vector<CardBase::id_t> available_cards;
    GET_STRING_ARRAY_MEMBER(available_cards, json, "available_cards");

    std::optional<std::string> in_response_to;
    GET_OPTIONAL_STRING_MEMBER(in_response_to, json, "in_response_to");

    return std::make_unique<CreateLobbyResponseMessage>(game_id, available_cards, in_response_to, message_id);
}

static std::unique_ptr<JoinLobbyBroadcastMessage>
parseJoinGameBroadcast(const Document &json, const std::string &game_id, const std::string &message_id)
{
    std::vector<shared::PlayerBase::id_t> players;
    GET_STRING_ARRAY_MEMBER(players, json, "players");

    return std::make_unique<JoinLobbyBroadcastMessage>(game_id, players, message_id);
}

static std::unique_ptr<StartGameBroadcastMessage>
parseStartGameMessage(const Document & /*json*/, const std::string &game_id, const std::string &message_id)
{
    return std::make_unique<StartGameBroadcastMessage>(game_id, message_id);
}

static std::unique_ptr<EndGameBroadcastMessage>
parseEndGameBroadcast(const Document & /*json*/, const std::string &game_id, const std::string &message_id)
{
    return std::make_unique<EndGameBroadcastMessage>(game_id, message_id);
}

static std::unique_ptr<ResultResponseMessage> parseResultResponse(const Document &json, const std::string &game_id,
                                                                  const std::string &message_id)
{
    std::optional<std::string> in_response_to;
    GET_OPTIONAL_STRING_MEMBER(in_response_to, json, "in_response_to");
    bool success;
    GET_BOOL_MEMBER(success, json, "success");
    std::optional<std::string> additional_information;
    GET_OPTIONAL_STRING_MEMBER(additional_information, json, "additional_information");

    return std::make_unique<ResultResponseMessage>(game_id, success, in_response_to, additional_information,
                                                   message_id);
}

static std::unique_ptr<ActionOrderMessage> parseActionOrder(const Document &json, const std::string &game_id,
                                                            const std::string &message_id)
{
    std::optional<std::string> description;

    if ( !json.HasMember("order") && !json["order"].IsObject() ) {
        return nullptr;
    }
    std::unique_ptr<ActionOrder> order = ActionOrder::fromJson(json["order"]);

    if ( !json.HasMember("game_state") ) {
        LOG(WARN) << "GameStateMessage: No game_state member";
        return nullptr;
    }
    std::unique_ptr<reduced::GameState> game_state = reduced::GameState::fromJson(json["game_state"]);

    GET_OPTIONAL_STRING_MEMBER(description, json, "description");

    return std::make_unique<ActionOrderMessage>(game_id, std::move(order), std::move(game_state), description,
                                                message_id);
}

namespace shared
{
    std::unique_ptr<ServerToClientMessage> ServerToClientMessage::fromJson(const std::string &json)
    {
        Document doc;
        doc.Parse(json.c_str());

        if ( doc.HasParseError() ) {
            return nullptr;
        }

        std::string game_id;
        GET_STRING_MEMBER(game_id, doc, "game_id");
        std::string message_id;
        GET_STRING_MEMBER(message_id, doc, "message_id");

        std::string type;
        GET_STRING_MEMBER(type, doc, "type");
        if ( type == "game_state" ) {
            return parseGameStateMessage(doc, game_id, message_id);
        } else if ( type == "initiate_game_response" ) {
            return parseCreateLobbyResponse(doc, game_id, message_id);
        } else if ( type == "join_game_broadcast" ) {
            return parseJoinGameBroadcast(doc, game_id, message_id);
        } else if ( type == "start_game_broadcast" ) {
            return parseStartGameMessage(doc, game_id, message_id);
        } else if ( type == "end_game_broadcast" ) {
            return parseEndGameBroadcast(doc, game_id, message_id);
        } else if ( type == "result_response" ) {
            return parseResultResponse(doc, game_id, message_id);
        } else if ( type == "action_order" ) {
            return parseActionOrder(doc, game_id, message_id);
        } else {
            return nullptr;
        }
    }
} // namespace shared

static std::unique_ptr<GameStateRequestMessage> parseGameStateRequest(const Document & /*json*/,
                                                                      const std::string &game_id,
                                                                      const PlayerBase::id_t &player_id,
                                                                      const std::string &message_id)
{
    return std::make_unique<GameStateRequestMessage>(game_id, player_id, message_id);
}

static std::unique_ptr<CreateLobbyRequestMessage> parseCreateLobbyRequest(const Document & /*json*/,
                                                                          const std::string &game_id,
                                                                          const PlayerBase::id_t &player_id,
                                                                          const std::string &message_id)
{
    return std::make_unique<CreateLobbyRequestMessage>(game_id, player_id, message_id);
}

static std::unique_ptr<JoinLobbyRequestMessage> parseJoinGameRequest(const Document & /*json*/,
                                                                     const std::string &game_id,
                                                                     const PlayerBase::id_t &player_id,
                                                                     const std::string &message_id)
{
    return std::make_unique<JoinLobbyRequestMessage>(game_id, player_id, message_id);
}

static std::unique_ptr<StartGameRequestMessage> parseStartGameRequest(const Document &json, const std::string &game_id,
                                                                      const PlayerBase::id_t &player_id,
                                                                      const std::string &message_id)
{
    std::vector<CardBase::id_t> selected_cards;
    GET_STRING_ARRAY_MEMBER(selected_cards, json, "selected_cards");
    if ( selected_cards.size() != 10 ) {
        return nullptr;
    }

    return std::make_unique<StartGameRequestMessage>(game_id, player_id, selected_cards, message_id);
}

static std::unique_ptr<ActionDecisionMessage> parseActionDecision(const Document &json, const std::string &game_id,
                                                                  const PlayerBase::id_t &player_id,
                                                                  const std::string &message_id)
{
    std::optional<std::string> in_response_to;
    GET_OPTIONAL_STRING_MEMBER(in_response_to, json, "in_response_to");

    ActionDecision *decision = nullptr;
    std::string action;
    GET_STRING_MEMBER(action, json, "action");
    if ( action == "play_action_card" ) {
        shared::CardBase::id_t card_id;
        shared::CardAccess from;
        GET_STRING_MEMBER(card_id, json, "card_id");
        GET_ENUM_MEMBER(from, json, "from", shared::CardAccess);
        decision = new PlayActionCardDecision(card_id, from);
    } else if ( action == "buy_card" ) {
        CardBase::id_t card;
        GET_STRING_MEMBER(card, json, "card");
        decision = new BuyCardDecision(card);
    } else if ( action == "end_action_phase" ) {
        decision = new EndActionPhaseDecision();
    } else if ( action == "end_turn" ) {
        decision = new EndTurnDecision();
    } else if ( action == "deck_choice" ) {
        std::vector<shared::CardBase::id_t> cards;
        std::vector<shared::DeckChoiceDecision::AllowedChoice> choices;

        GET_STRING_ARRAY_MEMBER(cards, json, "cards");
        GET_ENUM_ARRAY_MEMBER(choices, json, "choices", shared::DeckChoiceDecision::AllowedChoice);
        decision = new DeckChoiceDecision(cards, choices);
    } else if ( action == "board_choice" ) {
        shared::CardBase::id_t chosen_card;
        GET_STRING_MEMBER(chosen_card, json, "chosen_card");
        decision = new GainFromBoardDecision(chosen_card);
    } else {
        return nullptr;
    }

    return std::make_unique<ActionDecisionMessage>(game_id, player_id, std::unique_ptr<ActionDecision>(decision),
                                                   in_response_to, message_id);
}

/* ======= CLIENT TO SERVER MESSAGES ======= */

namespace shared
{
    std::unique_ptr<ClientToServerMessage> ClientToServerMessage::fromJson(const std::string &json)
    {
        Document doc;
        doc.Parse(json.c_str());

        if ( doc.HasParseError() ) {
            return nullptr;
        }

        std::string game_id;
        GET_STRING_MEMBER(game_id, doc, "game_id");
        std::string message_id;
        GET_STRING_MEMBER(message_id, doc, "message_id");
        std::string player_id;
        GET_STRING_MEMBER(player_id, doc, "player_id");

        std::string type;
        GET_STRING_MEMBER(type, doc, "type");
        if ( type == "game_state_request" ) {
            return parseGameStateRequest(doc, game_id, player_id, message_id);
        } else if ( type == "initiate_game_request" ) {
            return parseCreateLobbyRequest(doc, game_id, player_id, message_id);
        } else if ( type == "join_game_request" ) {
            return parseJoinGameRequest(doc, game_id, player_id, message_id);
        } else if ( type == "start_game_request" ) {
            return parseStartGameRequest(doc, game_id, player_id, message_id);
        } else if ( type == "action_decision" ) {
            return parseActionDecision(doc, game_id, player_id, message_id);
        } else {
            return nullptr;
        }
    }
} // namespace shared
