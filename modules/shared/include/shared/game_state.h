#pragma once

#include <string>
#include <vector>

namespace shared
{

    // Representing card types like this allows us to combine them using bitwise operations.
    // Example: `ACTION | ATTACK` will create a card that is both an action and an attack.
    enum CardType
    {
        ACTION = 0b1,
        ATTACK = 0b10,
        REACTION = 0b100,
        TREASURE = 0b1000,
        VICTORY = 0b10000,
        CURSE = 0b100000
    };

    class CardBase
    {
    public:
        using id_t = std::string;

        CardBase(id_t value, CardType type, unsigned int cost) : value(value), type(type), cost(cost) {}
        bool isAction() const;
        bool isAttack() const;
        bool isTreasure() const;
        bool isReaction() const;
        bool isVictory() const;
        bool isCurse() const;
        unsigned int getCost();
        CardType getType();
        id_t getId();
        virtual std::string toString() = 0;

    protected:
        id_t value;
        const CardType type;
        const unsigned int cost;
    };

    class PlayerBase
    {
    public:
        using id_t = std::string;
        id_t getId() { return id; }
        id_t getId() const { return id; }
        PlayerBase() {}
        PlayerBase(id_t player_id) : id(player_id) {}
        // TODO: initialize victory_points, available_actions, available_buys, available_treasure, current_card,
        // discard_pile, draw_pile_size

    protected:
        bool operator==(const PlayerBase &other) const;

        id_t id;
        unsigned int victory_points;
        std::vector<CardBase::id_t> played_cards;
        std::vector<CardBase::id_t> gained_cards;
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
        bool operator==(const ReducedEnemy &other) const;

    protected:
        unsigned int hand_size;
    };

    class ReducedPlayer : public PlayerBase
    {
    public:
        ReducedPlayer() {}
        ReducedPlayer(id_t player_id) : PlayerBase(player_id) {}
        bool operator==(const ReducedPlayer &other) const;

    protected:
        std::vector<CardBase::id_t> hand_cards;
    };

    struct Pile
    {
    public:
        Pile(CardBase::id_t card, unsigned int count) : card(card), count(count) {}
        bool operator==(const Pile &other) const;

        // TODO: Test these functions
        std::string to_json();
        static Pile *from_json(const std::string &json);

        CardBase::id_t card;
        unsigned int count;
    };

    class Board
    {
    public:
        bool operator==(const Board &other) const;

        std::vector<Pile> victory_cards;
        std::vector<Pile> treasure_cards;
        std::vector<Pile> kingdom_cards;
        std::vector<CardBase::id_t> trash;
    };

    class ReducedGameState
    {
    public:
        bool operator==(const ReducedGameState &other) const;

        ReducedGameState() {}
        Board board;
        ReducedPlayer player;
        std::vector<ReducedEnemy> enemies;
        PlayerBase::id_t active_player;
    };
} // namespace shared
