#include <shared/game/reduced/player.h>
#include <shared/utils/json.h>

namespace reduced
{
    Player::Player(const shared::PlayerBase &player, const std::vector<shared::CardBase::id_t> &hand_cards) :
        PlayerBase(player), hand_cards(std::move(hand_cards))
    {}

    Player::ptr_t Player::make(const shared::PlayerBase &player, std::vector<shared::CardBase::id_t> hand_cards)
    {
        return ptr_t(new Player(player, hand_cards));
    }

    rapidjson::Document Player::toJson() const
    {
        rapidjson::Document doc = PlayerBase::toJson();
        ADD_ARRAY_OF_STRINGS_MEMBER(this->hand_cards, hand_cards);
        return doc;
    }

    std::unique_ptr<Player> Player::fromJson(const rapidjson::Value &json)
    {
        std::unique_ptr<shared::PlayerBase> player_base = PlayerBase::fromJson(json);
        std::vector<shared::CardBase::id_t> hand_cards;
        GET_STRING_ARRAY_MEMBER(hand_cards, json, "hand_cards");
        return std::unique_ptr<Player>(new Player(*player_base, hand_cards));
    }

    const std::vector<shared::CardBase::id_t> &Player::getHandCards() const { return hand_cards; }

    Enemy::Enemy(const shared::PlayerBase &player, unsigned int hand) : shared::PlayerBase(player), hand_size(hand) {}

    Enemy::ptr_t Enemy::make(const shared::PlayerBase &player, unsigned int hand_size)
    {
        return ptr_t(new Enemy(player, hand_size));
    }

    rapidjson::Document Enemy::toJson() const
    {
        rapidjson::Document doc = shared::PlayerBase::toJson();
        ADD_UINT_MEMBER(this->hand_size, hand_size);
        return doc;
    }

    std::unique_ptr<Enemy> Enemy::fromJson(const rapidjson::Value &json)
    {
        std::unique_ptr<shared::PlayerBase> player_base = PlayerBase::fromJson(json);
        unsigned int hand_size;
        GET_UINT_MEMBER(hand_size, json, "hand_size");
        return std::unique_ptr<Enemy>(new Enemy(*player_base, hand_size));
    }

    unsigned int Enemy::getHandSize() const { return hand_size; }
} // namespace reduced