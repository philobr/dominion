#pragma once

#include <string>
#include <vector>

#include <iomanip> // for operator<<
#include <iostream> // for operator<<

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

        id_t getId() const { return player_id; }

        unsigned int getVictoryPoints() const { return victory_points; }
        unsigned int getActions() const { return available_actions; }
        unsigned int getBuys() const { return available_buys; }
        unsigned int getTreasure() const { return available_treasure; }

        void decActions();
        void decBuys();
        void decTreasure(const unsigned int dec_amount);

    protected:
        // these are not used, as played cards will be stored in the board
        // and gained cards are directly added to the discard pile, as per game rules
        /*
        std::vector<CardBase::id_t> played_cards;
        std::vector<CardBase::id_t> gained_cards;
        */

        id_t player_id;
        unsigned int victory_points;
        unsigned int available_actions;
        unsigned int available_buys;
        unsigned int available_treasure;
        CardBase::id_t current_card;
        std::pair<CardBase::id_t, unsigned int> discard_pile;
        unsigned int draw_pile_size;
    };

    class ReducedEnemy : public PlayerBase
    {
    public:
        ReducedEnemy(const PlayerBase &player, unsigned int hand) : PlayerBase(player), hand_size(hand) {}

    protected:
        unsigned int hand_size;
    };

    class ReducedPlayer : public PlayerBase
    {
    public:
        ReducedPlayer(const PlayerBase &player, std::vector<CardBase::id_t> hand) : PlayerBase(player), hand_cards(hand)
        {}

    protected:
        std::vector<CardBase::id_t> hand_cards;
    };

} // namespace shared