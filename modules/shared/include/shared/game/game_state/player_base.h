#pragma once

#include <string>
#include <vector>

#include <iomanip> // for operator<<
#include <iostream> // for operator<<
#include <memory>

#include <rapidjson/document.h>
#include <shared/game/cards/card_base.h>

namespace shared
{
    class PlayerBase
    {
    public:
        using id_t = std::string;

        PlayerBase(id_t player_id);
        PlayerBase(const PlayerBase &other);

        PlayerBase &operator=(const PlayerBase &other);

        PlayerBase(PlayerBase &&other) noexcept;
        PlayerBase &operator=(PlayerBase &&other) noexcept;

        virtual ~PlayerBase() = default;

        bool operator==(const PlayerBase &other) const;

        id_t getId() const { return player_id; }

        unsigned int getVictoryPoints() const { return victory_points; }
        unsigned int getActions() const { return actions; }
        unsigned int getBuys() const { return buys; }
        unsigned int getTreasure() const { return treasure; }

        /**
         * @brief Decrements actions by one, or keeps it at 0.
         */
        void decActions();

        /**
         * @brief Decrements buys by one, or keeps it at 0.
         */
        void decBuys();

        /**
         * @brief Decrements treasure by min(dec_amount, treasure_amount)
         *
         * @param dec_amount
         */
        void decTreasure(const unsigned int dec_amount);

    protected:
        id_t player_id;

        unsigned int victory_points;
        unsigned int actions;
        unsigned int buys;
        unsigned int treasure;

        CardBase::id_t current_card;
        std::vector<CardBase::id_t> discard_pile;
        unsigned int draw_pile_size;

        std::vector<CardBase::id_t> played_cards;

        /**
         * @brief Convert the player to a `rapidjson::Document` JSON object.
         */
        rapidjson::Document toJson() const;
        /**
         * @brief Initialize a player from a `rapidjson::Value` JSON object.
         */
        static std::unique_ptr<PlayerBase> fromJson(const rapidjson::Value &json);
    };

} // namespace shared


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
};