#pragma once

#include <vector>

#include <shared/game/cards/card_base.h>
#include <shared/game/game_state/board_base.h>

/**
 * @brief Returns 10 kingdom cards:
 *
 * "Village", "Smithy", "Market", "Festival",       "Laboratory",
 *
 * "Moat",    "Chapel", "Feast",  "Moneylender",    "Remodel"
 *
 * @return std::vector<shared::CardBase::id_t>
 */
std::vector<shared::CardBase::id_t> get_valid_kingdom_cards();

/**
 * @brief Set the count of min(n, pile_container.size()) piles to empty.
 *
 * @param pile_container
 * @param n
 */
void set_n_piles_to_empty(shared::Board::pile_container_t &pile_container, size_t n);