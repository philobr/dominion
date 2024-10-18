#pragma once

#include "player.h"
#include "board.h"

class ClientGameState {
    Player player;
    Board board;

public:
    ClientGameState();

    void assign_player(Player player);
    void init_board(const std::vector<Board::Pile>& cards);

    bool buy(card_name_t card);
    bool trash(card_name_t card);

    std::vector<card_name_t> draw(int n);
    bool discard(const std::vector<card_name_t>& cards);

    bool defend();
    bool attack(card_name_t card);
};
