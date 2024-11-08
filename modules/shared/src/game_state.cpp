
#include <rapidjson/document.h>
#include <shared/game_state.h>
#include <shared/utils/json.h>

using namespace rapidjson;
using namespace shared;

namespace shared
{

    std::string Pile::to_json()
    {
        Document doc;
        doc.SetObject();

        ADD_STRING_MEMBER(card.c_str(), card);
        ADD_UINT_MEMBER(count, count);

        return document_to_string(doc);
    }

    Pile *Pile::from_json(const std::string &json)
    {
        Document doc;
        doc.Parse(json.c_str());

        if ( doc.HasParseError() ) {
            return nullptr;
        }

        CardBase::id_t card;
        GET_STRING_MEMBER(card, doc, "card");
        unsigned int count;
        GET_UINT_MEMBER(count, doc, "count");

        return new Pile(card, count);
    }
} // namespace shared
