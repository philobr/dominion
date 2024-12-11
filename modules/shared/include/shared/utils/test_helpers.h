#pragma once

#include <vector>

#include <shared/game/cards/card_base.h>
#include <shared/game/game_state/board_base.h>
#include <shared/game/game_state/reduced_game_state.h>
#include <shared/game/reduced/player.h>

/**
 * @brief Returns 10 kingdom cards
 */
std::vector<shared::CardBase::id_t> getValidKingdomCards();

/**
 * @brief Set the count of min(n, pile_container.size()) piles to empty.
 */
void setNPilesToEmpty(shared::Board::pile_container_t &pile_container, size_t n);

namespace test_helper
{
    std::vector<shared::CardBase::id_t> getRandomTreasureCards(size_t n_cards);
    std::vector<shared::CardBase::id_t> getRandomVictoryCards(size_t n_cards);

    std::vector<shared::CardBase::id_t> getRandomKingdomCards(size_t n_cards);
    std::vector<shared::CardBase::id_t> getValidRandomKingdomCards(size_t n_cards);

    std::vector<shared::CardBase::id_t> getRandomCards(size_t n_victory, size_t n_treasure, size_t n_kingdom);
    std::vector<shared::CardBase::id_t> getRandomCards(size_t n_cards);

    reduced::Player::ptr_t getReducedPlayer(const std::string &name, std::vector<shared::CardBase::id_t> hand_cards);
    reduced::Player::ptr_t getReducedPlayer(const std::string &name);

    reduced::Enemy::ptr_t getReducedEnemy(const std::string &name, size_t n_hand_cards);
    std::vector<reduced::Enemy::ptr_t> getReducedEnemies(size_t n_enemies, std::vector<size_t> hand_cards);
    std::vector<reduced::Enemy::ptr_t> getReducedEnemies(size_t n_enemies);

    shared::Board::ptr_t getBoard(size_t n_players, std::vector<shared::CardBase::id_t> kingdom_cards);
    shared::Board::ptr_t getBoard(size_t n_players);

    reduced::GameState getReducedGameState(size_t n_players);
    reduced::GameState getReducedGameState(size_t n_players, std::vector<shared::CardBase::id_t> kingdom_cards,
                                           std::vector<shared::CardBase::id_t> hand_cards,
                                           std::vector<size_t> enemy_hand_cards);

    std::unique_ptr<reduced::GameState> getReducedGameStatePtr(size_t n_players);
    std::unique_ptr<reduced::GameState> getReducedGameStatePtr(size_t n_players,
                                                               std::vector<shared::CardBase::id_t> kingdom_cards,
                                                               std::vector<shared::CardBase::id_t> hand_cards,
                                                               std::vector<size_t> enemy_hand_cards);

} // namespace test_helper
