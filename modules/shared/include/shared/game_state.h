#pragma once

#include <memory>
#include <string>
#include <vector>

#include <shared/utils/json.h>

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
        id_t getId() const { return id; }
        PlayerBase(id_t player_id) : id(player_id) {}
        // TODO: initialize victory_points, available_actions, available_buys, available_treasure, current_card,
        // discard_pile, draw_pile_size

        // Make this class polymorphic
        virtual ~PlayerBase() = default;

    protected:
        PlayerBase() {}
        bool operator==(const PlayerBase &other) const;
        /**
         * @brief Converts the player object to a JSON object.
         *
         * The JSON object will be incomplete and must be completed by the calling child class.
         */
        rapidjson::Document to_json();
        /**
         * @brief Initializes the common player attributes from a JSON object.
         *
         * @param json The JSON object to initialize the player from.
         * @return true if the JSON object was successfully parsed, false otherwise.
         * If false is returned, the player object is left in an undefined state.
         */
        bool from_json(const rapidjson::Value &json);

        id_t id;
        unsigned int victory_points;
        std::vector<CardBase::id_t> played_cards;
        std::vector<CardBase::id_t> gained_cards;
        unsigned int available_actions;
        unsigned int available_buys;
        unsigned int available_treasure;
        CardBase::id_t current_card;
        std::vector<CardBase::id_t> discard_pile;
        unsigned int draw_pile_size;

    private:
        // TODO: Find a better way to do this
        /**
         * @brief Hacky way to make use of the parsing macros (`json.h`).
         * This function is not meant to be called except by the `from_json` function.
         */
        PlayerBase *_from_json_internal(const rapidjson::Value &json);
    };

    class ReducedEnemy : public PlayerBase
    {
    public:
        ReducedEnemy(id_t player_id, unsigned int hand_size) : PlayerBase(player_id), hand_size(hand_size) {}
        bool operator==(const ReducedEnemy &other) const;
        // TODO: Test this function
        rapidjson::Document to_json();
        // TODO: Test this function
        static std::unique_ptr<ReducedEnemy> from_json(const rapidjson::Value &json);

    protected:
        unsigned int hand_size;

    private:
        ReducedEnemy() : PlayerBase() {}
    };

    class ReducedPlayer : public PlayerBase
    {
    public:
        ReducedPlayer(id_t player_id, std::vector<CardBase::id_t> hand_cards) :
            PlayerBase(player_id), hand_cards(hand_cards)
        {}
        bool operator==(const ReducedPlayer &other) const;
        // TODO: Test this function
        rapidjson::Document to_json();
        // TODO: Test this function
        static std::unique_ptr<ReducedPlayer> from_json(const rapidjson::Value &json);

    protected:
        std::vector<CardBase::id_t> hand_cards;

    private:
        ReducedPlayer() : PlayerBase() {}
    };

    struct Pile
    {
    public:
        Pile(CardBase::id_t card, unsigned int count) : card(card), count(count) {}
        bool operator==(const Pile &other) const;

        rapidjson::Document to_json();
        static std::unique_ptr<Pile> from_json(const rapidjson::Value &json);

        CardBase::id_t card;
        unsigned int count;
    };

    class Board
    {
    public:
        /**
         * @brief Construct a new Board object
         *
         * @param kingdom_cards The kingdom cards that are available in this game (chosen by the game master).
         * They must be exactly 10 cards.
         */
        Board(const std::vector<CardBase::id_t> &kingdom_cards, unsigned int num_players);
        bool operator==(const Board &other) const;

        rapidjson::Document to_json();
        static std::unique_ptr<Board> from_json(const rapidjson::Value &json);

        std::vector<Pile> kingdom_cards;
        std::vector<Pile> victory_cards;
        std::vector<Pile> treasure_cards;
        std::vector<CardBase::id_t> trash;

    private:
        Board(std::vector<Pile> kingdom_cards, std::vector<Pile> victory_cards, std::vector<Pile> treasure_cards,
              std::vector<CardBase::id_t> trash) :
            kingdom_cards(kingdom_cards), victory_cards(victory_cards), treasure_cards(treasure_cards), trash(trash)
        {}
    };

    class ReducedGameState
    {
    public:
        ReducedGameState(Board board, ReducedPlayer player, std::vector<ReducedEnemy> enemies,
                         PlayerBase::id_t current_player) :
            board(board), player(player), enemies(enemies), current_player(current_player)
        {}
        bool operator==(const ReducedGameState &other) const;

        Board board;
        ReducedPlayer player;
        std::vector<ReducedEnemy> enemies;
        PlayerBase::id_t current_player;
    };
} // namespace shared
