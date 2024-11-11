
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

    // TODO: Implement PlayerBase
    // ask Nicola why some stuff is protected. ID cannot be accessed by the game logic
    // i think it should be private/accessible by the game logic
    // Maybe functions for get_reduced_player in playerBase etc...

    PlayerBase::id_t PlayerBase::getId() const { return player_id; }

    bool ReducedEnemy::operator==(const ReducedEnemy &other) const
    {
        return PlayerBase::operator==(other) && this->hand_size == other.hand_size;
    }

    bool ReducedPlayer::operator==(const ReducedPlayer &other) const
    {
        return PlayerBase::operator==(other) && this->hand_cards == other.hand_cards;
    }

    bool Pile::operator==(const Pile &other) const { return this->card == other.card && this->count == other.count; }

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

    bool ReducedGameState::operator==(const ReducedGameState &other) const
    {
        return this->board == other.board && this->player == other.player && this->enemies == other.enemies &&
                this->current_player == other.current_player;
    }

} // namespace shared
