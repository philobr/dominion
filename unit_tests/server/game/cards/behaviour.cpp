#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <tuple>

#include <server/game/cards/cards.h>

// Helper function to create a card with specified behaviors
template <typename CardType>
std::unique_ptr<CardType> createCard(const std::string &id, shared::CardType type, unsigned int cost)
{
    return std::make_unique<CardType>(id, type, cost);
}


shared::PlayerBase::id_t get_affected_player() { return "player1"; }

server::GameState get_gamestate()
{

    std::vector<shared::CardBase::id_t> playing_cards = {"card1", "card2", "card3", "card4", "card5",
                                                         "card6", "card7", "card8", "card9", "card10"};

    std::vector<shared::PlayerBase::id_t> players = {get_affected_player(), "player2", "player3"};
    server::GameState gamestate(playing_cards, players);
    return gamestate;
}

// Begin test cases

TEST(BehaviourTest, GainBuysTest)
{
    auto affected_player = get_affected_player();
    auto gamestate = get_gamestate();

    auto add_buys = server::behaviour::GainBuys<3>();

    auto prev_buys = gamestate.get_player(affected_player).getAvailableBuys();
    add_buys.apply(gamestate, affected_player);
    auto cur_buys = gamestate.get_player(affected_player).getAvailableBuys();

    ASSERT_EQ(prev_buys + 3, cur_buys);
}
