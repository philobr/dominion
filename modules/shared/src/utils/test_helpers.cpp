#include <shared/utils/test_helpers.h>

#include <cstdlib> // for std::rand

std::vector<shared::CardBase::id_t> getValidKingdomCards()
{
    return {"Village", "Smithy", "Market",  "Festival",    "Laboratory",
            "Moat",    "Chapel", "Artisan", "Moneylender", "Remodel"};
}

void setNPilesToEmpty(shared::Board::pile_container_t &pile_container, size_t n)
{
    size_t i = 0;
    for ( const auto &pile : pile_container ) {
        if ( i >= n ) {
            break;
        }
        pile.count = 0;
        ++i;
    }
}

namespace test_helper
{
    std::vector<shared::CardBase::id_t> getValidRandomKingdomCards(size_t n_cards)
    {
        std::vector<std::string> action_cards = {"Moat",
                                                 "Village",
                                                 "Smithy",
                                                 "Gardens",
                                                 "Festival",
                                                 "Market",
                                                 "Laboratory",
                                                 "Council_Room",
                                                 "Witch"
                                                 "Cellar",
                                                 "Chapel",
                                                 "Workshop",
                                                 "Vassal",
                                                 "Harbinger",
                                                 "Poacher",
                                                 "Moneylender",
                                                 "Militia",
                                                 "Bureaucrat",
                                                 "Remodel",
                                                 "Sentry",
                                                 "Library",
                                                 "Bandit",
                                                 "Mine",
                                                 "Artisan",
                                                 "Merchant",
                                                 "Throne_Room"};
        std::set<std::string> cards;

        while ( cards.size() < n_cards ) {
            size_t rand_idx = (static_cast<size_t>(std::rand()) % action_cards.size());
            cards.insert(action_cards[rand_idx]);
        }

        return {cards.begin(), cards.end()};
    }

    std::vector<shared::CardBase::id_t> getRandomVictoryCards(size_t n_cards)
    {
        std::vector<std::string> victory_cards = {"Estate", "Duchy", "Province", "Curse"};
        std::vector<shared::CardBase::id_t> cards;
        for ( size_t i = 0; i < n_cards; ++i ) {
            size_t rand_idx = (static_cast<size_t>(std::rand()) % victory_cards.size());
            cards.push_back(victory_cards[rand_idx]);
        }
        return cards;
    }

    std::vector<shared::CardBase::id_t> getRandomTreasureCards(size_t n_cards)
    {
        std::vector<std::string> treasure_cards = {"Copper", "Silver", "Gold"};
        std::vector<shared::CardBase::id_t> cards;
        for ( size_t i = 0; i < n_cards; ++i ) {
            size_t rand_idx = (static_cast<size_t>(std::rand()) % treasure_cards.size());
            cards.push_back(treasure_cards[rand_idx]);
        }
        return cards;
    }

    std::vector<shared::CardBase::id_t> getRandomKingdomCards(size_t n_cards)
    {
        std::vector<std::string> action_cards = {"Moat",
                                                 "Village",
                                                 "Smithy",
                                                 "Gardens",
                                                 "Festival",
                                                 "Market",
                                                 "Laboratory",
                                                 "Council_Room",
                                                 "Witch"
                                                 "Cellar",
                                                 "Chapel",
                                                 "Workshop",
                                                 "Vassal",
                                                 "Harbinger",
                                                 "Poacher",
                                                 "Moneylender",
                                                 "Militia",
                                                 "Bureaucrat",
                                                 "Remodel",
                                                 "Sentry",
                                                 "Library",
                                                 "Bandit",
                                                 "Mine",
                                                 "Artisan",
                                                 "Merchant",
                                                 "Throne_Room"};
        std::vector<shared::CardBase::id_t> cards;
        for ( size_t i = 0; i < n_cards; ++i ) {
            size_t rand_idx = (static_cast<size_t>(std::rand()) % action_cards.size());
            cards.push_back(action_cards[rand_idx]);
        }
        return cards;
    }

    std::vector<shared::CardBase::id_t> getRandomCards(size_t n_victory, size_t n_treasure, size_t n_kingdom)
    {
        auto random_cards = getRandomVictoryCards(n_victory);
        auto rand_treasure = getRandomTreasureCards(n_treasure);
        auto rand_kingdom = getRandomKingdomCards(n_kingdom);

        random_cards.insert(random_cards.end(), rand_treasure.begin(), rand_treasure.end());
        random_cards.insert(random_cards.end(), rand_kingdom.begin(), rand_kingdom.end());
        return random_cards;
    }

    std::vector<shared::CardBase::id_t> getRandomCards(size_t n_cards)
    {
        size_t n_victory = (static_cast<size_t>(std::rand()) % n_cards);
        size_t n_treasure = (static_cast<size_t>(std::rand()) % (n_cards - n_victory));
        size_t n_kingdom = (static_cast<size_t>(std::rand()) % (n_cards - n_victory - n_treasure));

        return getRandomCards(n_victory, n_treasure, n_kingdom);
    }

    reduced::Player::ptr_t getReducedPlayer(const std::string &name, std::vector<shared::CardBase::id_t> hand_cards)
    {
        shared::PlayerBase player_base(name);
        return reduced::Player::make(player_base, hand_cards);
    }

    reduced::Player::ptr_t getReducedPlayer(const std::string &name)
    {
        return getReducedPlayer(name, getRandomCards(5));
    }

    reduced::Enemy::ptr_t getReducedEnemy(const std::string &name, size_t n_hand_cards)
    {
        shared::PlayerBase player_base(name);
        return reduced::Enemy::make(player_base, n_hand_cards);
    }

    std::vector<reduced::Enemy::ptr_t> getReducedEnemies(size_t n_enemies)
    {
        std::vector<reduced::Enemy::ptr_t> result;
        for ( size_t i = 0; i < n_enemies; ++i ) {
            result.push_back(getReducedEnemy(std::string("reduced_enemy") + std::to_string(i + 1), 5));
        }
        return result;
    }

    shared::Board::ptr_t getBoard(size_t n_players, std::vector<shared::CardBase::id_t> kingdom_cards)
    {
        return shared::Board::make(kingdom_cards, n_players);
    }

    shared::Board::ptr_t getBoard(size_t n_players) { return getBoard(n_players, getValidRandomKingdomCards(10)); }

    reduced::GameState getReducedGameState(size_t n_players)
    {
        const std::string active_player = "reduced_player";
        return reduced::GameState(getBoard(n_players), getReducedPlayer(active_player),
                                  getReducedEnemies(n_players - 1), active_player);
    }
} // namespace test_helper