#pragma once

#include <string>
#include <vector>

#include <iomanip> // for operator<<
#include <iostream> // for operator<<
#include <memory>

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

        void decActions();
        void decBuys();
        void decTreasure(const unsigned int dec_amount);

    protected:
        id_t player_id;

        unsigned int victory_points;
        unsigned int actions;
        unsigned int buys;
        unsigned int treasure;

        CardBase::id_t current_card;
        std::pair<CardBase::id_t, unsigned int> discard_pile; // top card id, discard_pile size
        unsigned int draw_pile_size;

        std::vector<CardBase::id_t> played_cards;
    };

    class ReducedEnemy : public PlayerBase
    {
    public:
        using ptr_t = std::unique_ptr<ReducedEnemy>;

        static ptr_t make(const PlayerBase &player, unsigned int hand_size)
        {
            return ptr_t(new ReducedEnemy(player, hand_size));
        }

        unsigned int getHandSize() const { return hand_size; }

    protected:
        ReducedEnemy(const PlayerBase &player, unsigned int hand) : PlayerBase(player), hand_size(hand) {}

        unsigned int hand_size;
    };

    class ReducedPlayer : public PlayerBase
    {
    public:
        using ptr_t = std::unique_ptr<ReducedPlayer>;

        static ptr_t make(const PlayerBase &player, std::vector<CardBase::id_t> hand_cards)
        {
            return ptr_t(new ReducedPlayer(player, hand_cards));
        }

        const std::vector<CardBase::id_t> &getHandCards() const { return hand_cards; }

    protected:
        ReducedPlayer(const PlayerBase &player, const std::vector<CardBase::id_t> &hand_cards) :
            PlayerBase(player), hand_cards(std::move(hand_cards))
        {}

        const std::vector<CardBase::id_t> hand_cards;
    };

} // namespace shared