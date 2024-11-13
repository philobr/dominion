/**
 * @date 2024-11-11
 *
 * @brief This file includes all necessary files for cards to work.
 */

#pragma once

#include <server/game/cards/cards_core/behaviours.h>
#include <server/game/cards/cards_core/card_factory.h>

#define BUILD_CARD(id, type, cost, ...)                                                                                \
    struct id##_type_registration                                                                                      \
    {                                                                                                                  \
        id##_type_registration()                                                                                       \
        {                                                                                                              \
            using CardType = server::Card<__VA_ARGS__>;                                                                \
            auto card_ptr = std::make_unique<CardType>(#id, type, cost);                                               \
            server::CardFactory::registerCard(#id, std::move(card_ptr));                                               \
        }                                                                                                              \
    };                                                                                                                 \
    static id##_type_registration id##_instance;

#define BUILD_TREASURE_CARD(id, cost, coins)                                                                           \
    BUILD_CARD(id, shared::CardType::TREASURE, cost, server::behaviour::GainCoins<coins>)

#define BUILD_VICTORY_CARD(id, cost, points)                                                                           \
    BUILD_CARD(id, shared::CardType::VICTORY, cost, server::behaviour::GainPoints<points>)

#define BUILD_CURSE_CARD(id, cost) BUILD_CARD(id, shared::CardType::CURSE, cost, server::behaviour::GainPoints<-1>)