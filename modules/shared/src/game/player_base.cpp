
#include <shared/game/game_state/player_base.h>
#include <shared/utils/json.h>

namespace shared
{
    std::string toString(CardAccess val)
    {
        switch ( val ) {
            case DISCARD_PILE:
                return "\'DISCARD_PILE\'";
            case HAND:
                return "\'HAND\'";
            case DRAW_PILE_TOP:
                return "\'DRAW_PILE_TOP\'";
            case DRAW_PILE_BOTTOM:
                return "\'DRAW_PILE_BOTTOM\'";
            case TRASH:
                return "\'TRASH\'";
            case STAGED_CARDS:
                return "\'STAGED_CARDS\'";
            default:
                {
                    LOG(ERROR) << "CardAccess with value: " << val << " does not exist!";
                    throw exception::UnreachableCode();
                }
        }
    }

    PlayerBase::PlayerBase(id_t player_id) : player_id(player_id), actions(1), buys(1), treasure(0), draw_pile_size(0)
    {}

    PlayerBase::PlayerBase(const PlayerBase &other) = default;

    PlayerBase &PlayerBase::operator=(const PlayerBase &other)
    {
        if ( this != &other ) {
            player_id = other.player_id;
            actions = other.actions;
            buys = other.buys;
            treasure = other.treasure;
            current_card = other.current_card;
            discard_pile = other.discard_pile;
            draw_pile_size = other.draw_pile_size;
        }
        return *this;
    }

    PlayerBase::PlayerBase(PlayerBase &&other) noexcept :
        player_id(std::move(other.player_id)), actions(other.actions), buys(other.buys), treasure(other.treasure),
        current_card(std::move(other.current_card)), discard_pile(std::move(other.discard_pile)),
        draw_pile_size(other.draw_pile_size)
    {}

    PlayerBase &PlayerBase::operator=(PlayerBase &&other) noexcept
    {
        if ( this != &other ) {
            actions = other.actions;
            buys = other.buys;
            treasure = other.treasure;
            current_card = std::move(other.current_card);
            discard_pile = std::move(other.discard_pile);
            draw_pile_size = other.draw_pile_size;
        }
        return *this;
    }

    void PlayerBase::decActions()
    {
        if ( actions == 0 ) {
            return;
        }

        actions--;
    }

    void PlayerBase::decBuys()
    {
        if ( buys == 0 ) {
            return;
        }

        buys--;
    }

    void PlayerBase::decTreasure(const unsigned int dec_amount)
    {
        if ( treasure < dec_amount ) {
            return;
        }

        treasure -= dec_amount;
    }

    bool PlayerBase::operator==(const PlayerBase &other) const
    {
        return (player_id == other.player_id) && (actions == other.actions) && (buys == other.buys) &&
                (treasure == other.treasure) && (current_card == other.current_card) &&
                (discard_pile == other.discard_pile) && (draw_pile_size == other.draw_pile_size);
    }

    rapidjson::Document PlayerBase::toJson() const
    {
        rapidjson::Document doc;
        doc.SetObject();

        ADD_STRING_MEMBER(this->player_id.c_str(), player_id);
        ADD_UINT_MEMBER(this->actions, actions);
        ADD_UINT_MEMBER(this->buys, buys);
        ADD_UINT_MEMBER(this->treasure, treasure);
        ADD_STRING_MEMBER(this->current_card.c_str(), current_card);
        ADD_ARRAY_OF_STRINGS_MEMBER(this->discard_pile, discard_pile);
        ADD_UINT_MEMBER(this->draw_pile_size, draw_pile_size);

        return doc;
    }

    std::unique_ptr<PlayerBase> PlayerBase::fromJson(const rapidjson::Value &json)
    {
        PlayerBase::id_t player_id;
        GET_STRING_MEMBER(player_id, json, "player_id");
        std::unique_ptr<PlayerBase> player(new PlayerBase(player_id));
        GET_UINT_MEMBER(player->actions, json, "actions");
        GET_UINT_MEMBER(player->buys, json, "buys");
        GET_UINT_MEMBER(player->treasure, json, "treasure");
        GET_STRING_MEMBER(player->current_card, json, "current_card");
        GET_STRING_ARRAY_MEMBER(player->discard_pile, json, "discard_pile");
        GET_UINT_MEMBER(player->draw_pile_size, json, "draw_pile_size");
        return player;
    }

} // namespace shared
