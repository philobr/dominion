#include <server/game/cards/cards.h>

// this is needed to instantiate the card_factory at compiletime
server::CardFactory::map_t server::CardFactory::card_map_m;

// ================
// TREASURE CARDS
// ================

BUILD_TREASURE_CARD(Copper, 0, 1);
BUILD_TREASURE_CARD(Silver, 3, 2);
BUILD_TREASURE_CARD(Gold, 6, 3);

// ================
// VICTORY CARDS
// ================
BUILD_VICTORY_CARD(Estate, 2, 1);
BUILD_VICTORY_CARD(Duchy, 5, 3);
BUILD_VICTORY_CARD(Province, 8, 6);

// ================
// CURSE CARD
// ================
BUILD_CURSE_CARD(Curse, 0);

// ================
// ACTION CARDS
// ================
