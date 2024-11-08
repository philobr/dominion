
#include <memory>

#include <rapidjson/document.h>

#include <shared/message_types.h>
#include <shared/utils/json.h>

using namespace shared;
using namespace rapidjson;

/* ======= SERVER TO CLIENT MESSAGES ======= */

static std::unique_ptr<GameStateMessage> parse_game_state_message(const Document &json, const std::string &game_id,
                                                                  const std::string &message_id)
{
    std::unique_ptr<GameStateMessage> message = std::make_unique<GameStateMessage>();

    message->game_id = game_id;
    message->message_id = message_id;
    GET_OPTIONAL_STRING_MEMBER(message->in_response_to, json, "in_response_to");

    // TODO parse game state
    // message->game_state = <...>

    return message;
}

static std::unique_ptr<CreateLobbyResponseMessage>
parse_create_lobby_response(const Document &json, const std::string &game_id, const std::string &message_id)
{
    std::vector<shared::CardBase::id_t> available_cards; // TODO implement available cards
    GET_STRING_ARRAY_MEMBER(available_cards, json, "available_cards");
    std::unique_ptr<CreateLobbyResponseMessage> message = std::make_unique<CreateLobbyResponseMessage>(available_cards);

    message->game_id = game_id;
    message->message_id = message_id;
    GET_OPTIONAL_STRING_MEMBER(message->in_response_to, json, "in_response_to");

    return message;
}

static std::unique_ptr<JoinLobbyBroadcastMessage>
parse_join_game_broadcast(const Document &json, const std::string &game_id, const std::string &message_id)
{
    shared::PlayerBase::id_t player_id;
    GET_STRING_MEMBER(player_id, json, "player_id");
    std::unique_ptr<JoinLobbyBroadcastMessage> message = std::make_unique<JoinLobbyBroadcastMessage>(player_id);
    message->game_id = game_id;
    message->message_id = message_id;
    return message;
}

static std::unique_ptr<StartGameBroadcastMessage>
parse_start_game_message(const Document &json, const std::string &game_id, const std::string &message_id)
{
    std::unique_ptr<StartGameBroadcastMessage> message = std::make_unique<StartGameBroadcastMessage>();

    message->game_id = game_id;
    message->message_id = message_id;

    return message;
}

static std::unique_ptr<EndGameBroadcastMessage>
parse_end_game_broadcast(const Document &json, const std::string &game_id, const std::string &message_id)
{
    std::unique_ptr<EndGameBroadcastMessage> message = std::make_unique<EndGameBroadcastMessage>();

    message->game_id = game_id;
    message->message_id = message_id;

    return message;
}

static std::unique_ptr<ResultResponseMessage> parse_result_response(const Document &json, const std::string &game_id,
                                                                    const std::string &message_id)
{
    std::optional<std::string> in_response_to;
    bool success;
    std::optional<std::string> additional_information;

    GET_OPTIONAL_STRING_MEMBER(in_response_to, json, "in_response_to");
    GET_BOOL_MEMBER(success, json, "success");
    GET_OPTIONAL_STRING_MEMBER(additional_information, json, "additional_information");

    std::unique_ptr<ResultResponseMessage> message =
            std::make_unique<ResultResponseMessage>(success, in_response_to, additional_information);

    message->game_id = game_id;
    message->message_id = message_id;
    return message;
}

static std::unique_ptr<ActionOrderMessage> parse_action_order(const Document &json, const std::string &game_id,
                                                              const std::string &message_id)
{
    std::unique_ptr<ActionOrderMessage> message = std::make_unique<ActionOrderMessage>();

    message->game_id = game_id;
    message->message_id = message_id;
    GET_OPTIONAL_STRING_MEMBER(message->description, json, "description");

    return message;
}

namespace shared
{
    std::unique_ptr<ServerToClientMessage> ServerToClientMessage::from_json(const std::string &json)
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
            return parse_game_state_message(doc, game_id, message_id);
        } else if ( type == "initiate_game_response" ) {
            return parse_create_lobby_response(doc, game_id, message_id);
        } else if ( type == "join_game_broadcast" ) {
            return parse_join_game_broadcast(doc, game_id, message_id);
        } else if ( type == "start_game_broadcast" ) {
            return parse_start_game_message(doc, game_id, message_id);
        } else if ( type == "end_game_broadcast" ) {
            return parse_end_game_broadcast(doc, game_id, message_id);
        } else if ( type == "result_response" ) {
            return parse_result_response(doc, game_id, message_id);
        } else if ( type == "action_order" ) {
            return parse_action_order(doc, game_id, message_id);
        } else {
            return nullptr;
        }
    }
} // namespace shared

static std::unique_ptr<GameStateRequestMessage>
parse_game_state_request(const Document &json, const std::string &game_id, const std::string &message_id)
{
    std::unique_ptr<GameStateRequestMessage> message = std::make_unique<GameStateRequestMessage>();

    message->game_id = game_id;
    message->message_id = message_id;

    return message;
}

static std::unique_ptr<CreateLobbyRequestMessage>
parse_create_lobby_request(const Document &json, const std::string &game_id, const std::string &message_id)
{
    std::unique_ptr<CreateLobbyRequestMessage> message = std::make_unique<CreateLobbyRequestMessage>();

    message->game_id = game_id;
    message->message_id = message_id;
    GET_STRING_MEMBER(message->player_id, json, "player_id");

    return message;
}

static std::unique_ptr<JoinLobbyRequestMessage>
parse_join_game_request(const Document &json, const std::string &game_id, const std::string &message_id)
{
    std::unique_ptr<JoinLobbyRequestMessage> message = std::make_unique<JoinLobbyRequestMessage>();

    message->game_id = game_id;
    message->message_id = message_id;
    GET_STRING_MEMBER(message->player_id, json, "player_id");

    return message;
}

static std::unique_ptr<StartGameRequestMessage>
parse_start_game_request(const Document &json, const std::string &game_id, const std::string &message_id)
{
    std::unique_ptr<StartGameRequestMessage> message = std::make_unique<StartGameRequestMessage>();

    message->game_id = game_id;
    message->message_id = message_id;
    GET_STRING_ARRAY_MEMBER(message->selected_cards, json, "selected_cards");
    if ( message->selected_cards.size() != 10 ) {
        return nullptr;
    }

    return message;
}

static std::unique_ptr<ActionDecisionMessage> parse_action_decision(const Document &json, const std::string &game_id,
                                                                    const std::string &message_id)
{
    std::unique_ptr<ActionDecisionMessage> message = std::make_unique<ActionDecisionMessage>();

    message->game_id = game_id;
    message->message_id = message_id;
    GET_OPTIONAL_STRING_MEMBER(message->in_response_to, json, "in_response_to");
    GET_STRING_MEMBER(message->player_id, json, "player_id");

    ActionDecision *decision = nullptr;
    std::string action;
    GET_STRING_MEMBER(action, json, "action");
    if ( action == "play_action_card" ) {
        unsigned int cardIndex;
        GET_UINT_MEMBER(cardIndex, json, "card_index");
        decision = new PlayActionCardDecision(cardIndex);
    } else if ( action == "buy_card" ) {
        CardBase::id_t card;
        GET_STRING_MEMBER(card, json, "card");
        decision = new BuyCardDecision(card);
    } else if ( action == "end_turn" ) {
        decision = new EndTurnDecision();
    } else if ( action == "choose_n_cards_from_hand" ) {
        std::vector<unsigned int> cards;
        GET_UINT_ARRAY_MEMBER(cards, json, "cards");
        decision = new ChooseNCardsFromHandDecision(cards);
    } else {
        return nullptr;
    }

    message->decision = std::unique_ptr<ActionDecision>(decision);

    return message;
}

/* ======= CLIENT TO SERVER MESSAGES ======= */

namespace shared
{
    std::unique_ptr<ClientToServerMessage> ClientToServerMessage::from_json(const std::string &json)
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
        if ( type == "game_state_request" ) {
            return parse_game_state_request(doc, game_id, message_id);
        } else if ( type == "initiate_game_request" ) {
            return parse_create_lobby_request(doc, game_id, message_id);
        } else if ( type == "join_game_request" ) {
            return parse_join_game_request(doc, game_id, message_id);
        } else if ( type == "start_game_request" ) {
            return parse_start_game_request(doc, game_id, message_id);
        } else if ( type == "action_decision" ) {
            return parse_action_decision(doc, game_id, message_id);
        } else {
            return nullptr;
        }
    }
} // namespace shared
