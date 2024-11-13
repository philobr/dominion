#pragma once

#include <string>
#include <vector>

#include <shared/game/cards/card_base.h>
namespace shared
{
    class PlayerBase
    {
    public:
        using id_t = std::string;

        PlayerBase();
        PlayerBase(id_t player_id);
        PlayerBase(const PlayerBase &other);

        // TODO: initialize victory_points, available_actions, available_buys, available_treasure, current_card,
        // discard_pile, draw_pile_size
        // i think this is done by the copy constructor

        id_t getId() const { return player_id; }

        unsigned int getVictoryPoints() const { return victory_points; }
        unsigned int getAvailableActions() const { return available_actions; }
        unsigned int getAvailableBuys() const { return available_buys; }
        unsigned int getAvailableTreasure() const { return available_treasure; }

        void decreaseAvailableActions()
        {
            if ( available_actions == 0 ) {
                // should we throw here?
                return;
            }
            available_actions--;
        }

        void decreaseAvailableBuys()
        {
            if ( available_buys == 0 ) {
                // should we throw here?
                return;
            }
            available_buys--;
        }

        void decreaseAvailableTreasure(const unsigned int cost)
        {
            if ( available_treasure < cost ) {
                // should we throw here?
                return;
            }
            available_treasure = available_treasure - cost;
        }

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
        ReducedEnemy() {};
        // Constructor to use on the server side
        ReducedEnemy(unsigned int hand) : PlayerBase() { hand_size = hand; }
        ReducedEnemy(const PlayerBase &player, unsigned int hand) : PlayerBase(player), hand_size(hand) {}

    protected:
        unsigned int hand_size;
    };

    class ReducedPlayer : public PlayerBase
    {
    public:
        ReducedPlayer() {};
        // Constructor to use on the server side
        ReducedPlayer(std::vector<CardBase::id_t> hand) : PlayerBase() { hand_cards = hand; }
        ReducedPlayer(const PlayerBase &player, std::vector<CardBase::id_t> hand) : PlayerBase(player), hand_cards(hand)
        {}

    protected:
        std::vector<CardBase::id_t> hand_cards;
    };

} // namespace shared