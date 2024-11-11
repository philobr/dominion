
#include <memory>

#include <rapidjson/document.h>
#include <shared/game_state.h>
#include <shared/utils/json.h>

using namespace rapidjson;
using namespace shared;

namespace shared
{
    bool CardBase::isAction() const { return type & ACTION; }

    bool CardBase::isAttack() const { return type & ATTACK; }

    bool CardBase::isTreasure() const { return type & TREASURE; }

    bool CardBase::isReaction() const { return type & REACTION; }

    bool CardBase::isVictory() const { return type & VICTORY; }

    bool CardBase::isCurse() const { return type & CURSE; }

    unsigned int CardBase::getCost() { return cost; }

    CardType CardBase::getType() { return type; }

    CardBase::id_t CardBase::getId() { return id; }
    bool PlayerBase::operator==(const PlayerBase &other) const
    {
        return this->player_id == other.player_id && this->victory_points == other.victory_points &&
                this->played_cards == other.played_cards && this->gained_cards == other.gained_cards &&
                this->available_actions == other.available_actions && this->available_buys == other.available_buys &&
                this->available_treasure == other.available_treasure && this->current_card == other.current_card &&
                this->discard_pile == other.discard_pile && this->draw_pile_size == other.draw_pile_size;
    }

    rapidjson::Document PlayerBase::to_json()
    {
        Document doc;
        doc.SetObject();

        ADD_STRING_MEMBER(player_id.c_str(), player_id);
        ADD_UINT_MEMBER(victory_points, victory_points);
        ADD_ARRAY_OF_STRINGS_MEMBER(played_cards, played_cards);
        ADD_ARRAY_OF_STRINGS_MEMBER(gained_cards, gained_cards);
        ADD_UINT_MEMBER(available_actions, available_actions);
        ADD_UINT_MEMBER(available_buys, available_buys);
        ADD_UINT_MEMBER(available_treasure, available_treasure);
        ADD_STRING_MEMBER(current_card.c_str(), current_card);
        ADD_ARRAY_OF_STRINGS_MEMBER(discard_pile, discard_pile);
        ADD_UINT_MEMBER(draw_pile_size, draw_pile_size);

        return doc;
    }

    // This helper function is required in order to use the macros which return nullptr on failure.
    // Thus, any function that uses the macros must return some pointer type.
    // `PlayerBase::from_json` returns a bool, so we need to wrap the call to the macros in this function.
    PlayerBase *PlayerBase::_from_json_internal(const rapidjson::Value &json)
    {
        GET_STRING_MEMBER(player_id, json, "player_id");
        GET_UINT_MEMBER(victory_points, json, "victory_points");
        GET_STRING_ARRAY_MEMBER(played_cards, json, "played_cards");
        GET_STRING_ARRAY_MEMBER(gained_cards, json, "gained_cards");
        GET_UINT_MEMBER(available_actions, json, "available_actions");
        GET_UINT_MEMBER(available_buys, json, "available_buys");
        GET_UINT_MEMBER(available_treasure, json, "available_treasure");
        GET_STRING_MEMBER(current_card, json, "current_card");
        GET_STRING_ARRAY_MEMBER(discard_pile, json, "discard_pile");
        GET_UINT_MEMBER(draw_pile_size, json, "draw_pile_size");

        return this;
    }

    bool PlayerBase::from_json(const rapidjson::Value &json) { return _from_json_internal(json) != nullptr; }
    // TODO: Implement PlayerBase
    // ask Nicola why some stuff is protected. ID cannot be accessed by the game logic
    // i think it should be private/accessible by the game logic
    // Maybe functions for get_reduced_player in playerBase etc...

    PlayerBase::id_t PlayerBase::getId() const { return player_id; }

    bool ReducedEnemy::operator==(const ReducedEnemy &other) const
    {
        return PlayerBase::operator==(other) && this->hand_size == other.hand_size;
    }

    rapidjson::Document ReducedEnemy::to_json()
    {
        Document doc = PlayerBase::to_json();
        ADD_UINT_MEMBER(hand_size, hand_size);
        return doc;
    }

    std::unique_ptr<ReducedEnemy> ReducedEnemy::from_json(const rapidjson::Value &json)
    {
        std::unique_ptr<ReducedEnemy> enemy(new ReducedEnemy());
        if ( !enemy->PlayerBase::from_json(json) ) {
            return nullptr;
        }
        GET_UINT_MEMBER(enemy->hand_size, json, "hand_size");
        return enemy;
    }

    bool ReducedPlayer::operator==(const ReducedPlayer &other) const
    {
        return PlayerBase::operator==(other) && this->hand_cards == other.hand_cards;
    }

    rapidjson::Document ReducedPlayer::to_json()
    {
        Document doc = PlayerBase::to_json();
        ADD_ARRAY_OF_STRINGS_MEMBER(hand_cards, hand_cards);
        return doc;
    }

    std::unique_ptr<ReducedPlayer> ReducedPlayer::from_json(const rapidjson::Value &json)
    {
        std::unique_ptr<ReducedPlayer> player(new ReducedPlayer());
        if ( !player->PlayerBase::from_json(json) ) {
            return nullptr;
        }
        GET_STRING_ARRAY_MEMBER(player->hand_cards, json, "hand_cards");
        return player;
    }

    bool Pile::operator==(const Pile &other) const { return this->card == other.card && this->count == other.count; }

    rapidjson::Document Pile::to_json()
    {
        Document doc;
        doc.SetObject();

        ADD_STRING_MEMBER(card.c_str(), card);
        ADD_UINT_MEMBER(count, count);

        return doc;
    }

    std::unique_ptr<Pile> Pile::from_json(const rapidjson::Value &json)
    {
        CardBase::id_t card;
        GET_STRING_MEMBER(card, json, "card");
        unsigned int count;
        GET_UINT_MEMBER(count, json, "count");

        return std::make_unique<Pile>(card, count);
    }

    Board::Board(const std::vector<CardBase::id_t> &kingdom_cards, unsigned int num_players)
    {
        for ( auto card : kingdom_cards ) {
            this->kingdom_cards.push_back(Pile(card, 10));
        }

        this->treasure_cards.push_back(Pile("Copper", 60 - 7 * num_players));
        this->treasure_cards.push_back(Pile("Silver", 40));
        this->treasure_cards.push_back(Pile("Gold", 30));

        unsigned int num_estates = num_players == 2 ? 8 : 12;
        this->victory_cards.push_back(Pile("Estate", num_estates));
        this->victory_cards.push_back(Pile("Duchy", 12));
        this->victory_cards.push_back(Pile("Province", 12));

        this->victory_cards.push_back(Pile("Curse", 10 * (num_players - 1)));
    }

    bool Board::operator==(const Board &other) const
    {
        return this->victory_cards == other.victory_cards && this->treasure_cards == other.treasure_cards &&
                this->kingdom_cards == other.kingdom_cards && this->trash == other.trash;
    }

    rapidjson::Document Board::to_json()
    {
        Document doc;
        doc.SetObject();

#define ADD_PILES_MEMBER(name)                                                                                         \
    Value name(kArrayType);                                                                                            \
    for ( Pile pile : this->name ) {                                                                                   \
        name.PushBack(Value(pile.to_json(), doc.GetAllocator()), doc.GetAllocator());                                  \
    }                                                                                                                  \
    doc.AddMember(#name, name, doc.GetAllocator());

        ADD_PILES_MEMBER(kingdom_cards);
        ADD_PILES_MEMBER(victory_cards);
        ADD_PILES_MEMBER(treasure_cards);
        ADD_ARRAY_OF_STRINGS_MEMBER(trash, trash);

        return doc;
    }

    std::unique_ptr<Board> Board::from_json(const rapidjson::Value &json)
    {
#define GET_PILES_MEMBER(var, json, member)                                                                            \
    if ( !json.HasMember(member) || !json[member].IsArray() ) {                                                        \
        return nullptr;                                                                                                \
    }                                                                                                                  \
    std::vector<Pile> var;                                                                                             \
    for ( auto &pile : json[member].GetArray() ) {                                                                     \
        std::unique_ptr<Pile> p = Pile::from_json(pile);                                                               \
        if ( p == nullptr ) {                                                                                          \
            return nullptr;                                                                                            \
        }                                                                                                              \
        var.push_back(*p);                                                                                             \
    }

        GET_PILES_MEMBER(kingdom_cards_v, json, "kingdom_cards");
        GET_PILES_MEMBER(victory_cards_v, json, "victory_cards");
        GET_PILES_MEMBER(treasure_cards_v, json, "treasure_cards");
        std::vector<std::string> trash_v;
        GET_STRING_ARRAY_MEMBER(trash_v, json, "trash");

        return std::unique_ptr<Board>(new Board(kingdom_cards_v, victory_cards_v, treasure_cards_v, trash_v));
    }

    bool ReducedGameState::operator==(const ReducedGameState &other) const
    {
        return this->board == other.board && this->player == other.player && this->enemies == other.enemies &&
                this->current_player == other.current_player;
    }

} // namespace shared
