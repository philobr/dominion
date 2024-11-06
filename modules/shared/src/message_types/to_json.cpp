
#include <memory>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <shared/message_types.h>

#define ADD_STRING_MEMBER(var, key)                                                                                    \
    Value key##_value;                                                                                                 \
    key##_value.SetString(var.c_str(), doc.GetAllocator());                                                            \
    doc.AddMember(#key, key##_value, doc.GetAllocator());

#define ADD_OPTIONAL_STRING_MEMBER(var, key)                                                                           \
    if ( var ) {                                                                                                       \
        Value key##_value;                                                                                             \
        key##_value.SetString(var.value().c_str(), doc.GetAllocator());                                                \
        doc.AddMember(#key, key##_value, doc.GetAllocator());                                                          \
    }

#define ADD_BOOL_MEMBER(var, key)                                                                                      \
    Value key##_value;                                                                                                 \
    key##_value.SetBool(var);                                                                                          \
    doc.AddMember(#key, key##_value, doc.GetAllocator());

#define ADD_ARRAY_OF_STRINGS_MEMBER(var, key)                                                                          \
    Value key##_array(kArrayType);                                                                                     \
    for ( const auto &item : var ) {                                                                                   \
        Value item_value;                                                                                              \
        item_value.SetString(item.c_str(), doc.GetAllocator());                                                        \
        key##_array.PushBack(item_value, doc.GetAllocator());                                                          \
    }                                                                                                                  \
    doc.AddMember(#key, key##_array, doc.GetAllocator());

using namespace rapidjson;

Document document_from_common_attributes(const std::string &type, const std::string &game_id,
                                         const std::string &message_id)
{
    Document doc;
    doc.SetObject();

    ADD_STRING_MEMBER(type, type);
    ADD_STRING_MEMBER(game_id, game_id);
    ADD_STRING_MEMBER(message_id, message_id);

    return doc;
}

std::string document_to_string(Document &doc)
{
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    doc.Accept(writer);
    return buffer.GetString();
}


namespace shared
{

    // ======= SERVER TO CLIENT MESSAGES ======= //

    std::string GameStateMessage::to_json()
    {
        Document doc = document_from_common_attributes("game_state", this->game_id, this->message_id);
        ADD_OPTIONAL_STRING_MEMBER(this->in_response_to, in_response_to);
        // TODO: Add game state to the document
        return document_to_string(doc);
    }

    std::string CreateLobbyResponseMessage::to_json()
    {
        Document doc = document_from_common_attributes("initiate_game_response", this->game_id, this->message_id);
        ADD_OPTIONAL_STRING_MEMBER(this->in_response_to, in_response_to);
        ADD_ARRAY_OF_STRINGS_MEMBER(this->available_cards, available_cards);
        return document_to_string(doc);
    }

    std::string JoinLobbyBroadcastMessage::to_json()
    {
        Document doc = document_from_common_attributes("join_game_broadcast", this->game_id, this->message_id);
        ADD_STRING_MEMBER(this->player_id, player_id);
        return document_to_string(doc);
    }

    std::string StartGameBroadcastMessage::to_json()
    {
        Document doc = document_from_common_attributes("start_game_broadcast", this->game_id, this->message_id);
        return document_to_string(doc);
    }

    std::string EndGameBroadcastMessage::to_json()
    {
        Document doc = document_from_common_attributes("end_game_broadcast", this->game_id, this->message_id);
        return document_to_string(doc);
    }

    std::string ResultResponseMessage::to_json()
    {
        Document doc = document_from_common_attributes("result_response", this->game_id, this->message_id);
        ADD_OPTIONAL_STRING_MEMBER(this->in_response_to, in_response_to);
        ADD_BOOL_MEMBER(this->success, success);
        ADD_OPTIONAL_STRING_MEMBER(this->additional_information, additional_information);
        return document_to_string(doc);
    }

    std::string ActionOrderMessage::to_json()
    {
        Document doc = document_from_common_attributes("action_order", this->game_id, this->message_id);
        ADD_OPTIONAL_STRING_MEMBER(this->description, description);
        return document_to_string(doc);
    }

    // ======= CLIENT TO SERVER MESSAGES ======= //

    std::string GameStateRequestMessage::to_json()
    {
        Document doc = document_from_common_attributes("game_state_request", this->game_id, this->message_id);
        return document_to_string(doc);
    }

    std::string CreateLobbyRequestMessage::to_json()
    {
        Document doc = document_from_common_attributes("initiate_game_request", this->game_id, this->message_id);
        ADD_STRING_MEMBER(this->player_id, player_id);
        return document_to_string(doc);
    }

    std::string JoinLobbyRequestMessage::to_json()
    {
        Document doc = document_from_common_attributes("join_game_request", this->game_id, this->message_id);
        ADD_STRING_MEMBER(this->player_id, player_id);
        return document_to_string(doc);
    }

    std::string StartGameRequestMessage::to_json()
    {
        Document doc = document_from_common_attributes("start_game_request", this->game_id, this->message_id);
        ADD_ARRAY_OF_STRINGS_MEMBER(this->selected_cards, selected_cards);
        return document_to_string(doc);
    }

    std::string ActionDecisionMessage::to_json()
    {
        Document doc = document_from_common_attributes("action_decision", this->game_id, this->message_id);
        return document_to_string(doc);
    }

} // namespace shared
