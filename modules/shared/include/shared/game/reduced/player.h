#pragma once

#include <shared/game/game_state/player_base.h>

namespace reduced
{
    class Enemy : public shared::PlayerBase
    {
    public:
        using ptr_t = std::unique_ptr<Enemy>;

        static ptr_t make(const PlayerBase &player, unsigned int hand_size);

        Enemy(Enemy &&other) noexcept : PlayerBase(std::move(other)), hand_size(other.hand_size) {}

        rapidjson::Document toJson() const;
        static std::unique_ptr<Enemy> fromJson(const rapidjson::Value &json);

        unsigned int getHandSize() const;

    protected:
        Enemy(const shared::PlayerBase &player, unsigned int hand);
        unsigned int hand_size;
    };

    class Player : public shared::PlayerBase
    {
    public:
        using ptr_t = std::unique_ptr<Player>;

        static ptr_t make(const shared::PlayerBase &player, std::vector<shared::CardBase::id_t> hand_cards);

        Player(Player &&other) noexcept : shared::PlayerBase(std::move(other)), hand_cards(std::move(other.hand_cards))
        {}

        rapidjson::Document toJson() const;
        static std::unique_ptr<Player> fromJson(const rapidjson::Value &json);

        const std::vector<shared::CardBase::id_t> &getHandCards() const;

    protected:
        Player(const shared::PlayerBase &player, const std::vector<shared::CardBase::id_t> &hand_cards);
        const std::vector<shared::CardBase::id_t> hand_cards;
    };
}; // namespace reduced