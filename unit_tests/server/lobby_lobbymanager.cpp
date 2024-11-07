#include <server/lobbies.h>
#include <server/message_interface.h>
#include <shared/game_state.h>
#include <shared/message_types.h>
#include "gtest/gtest.h"

TEST(ServerLibraryTest, CreateLobby)
{
    server::MessageInterface message_interface;
    server::LobbyManager lobby_manager(message_interface);
    shared::PlayerBase::id_t player_1 = "Max";
    shared::PlayerBase::id_t player_2 = "Peter";

    shared::CreateLobbyRequestMessage request1;
    request1.game_id = "123";
    request1.message_id = "456";
    request1.player_id = player_1;
    
    shared::CreateLobbyRequestMessage request2;
    request2.game_id = "123";
    request2.message_id = "789";
    request2.player_id = player_2;
    
    auto games = lobby_manager.get_games();
    ASSERT_EQ(games->empty(), true) << "LobbyManager should be empty at the beginning";
    
    lobby_manager.create_lobby(request1);
    ASSERT_EQ(games->size(), 1) << "LobbyManager should contain one lobby after creating one";
    ASSERT_EQ(games->find("123") != games->end(), true) << "Lobby with id 123 should exist";
    ASSERT_EQ(games->at("123").get_game_master(), player_1) << "Game master should be player_1";
    ASSERT_EQ(games->at("123").get_full_game_state().get_players().size(), 1) << "There should be one player in the lobby";
    
    // No new lobby should be created, because game with id 123 already exists
    lobby_manager.create_lobby(request2);
    ASSERT_EQ(games->size(), 1);
    // Check if the lobby with id really 123 exists
    ASSERT_EQ(games->find("123") != games->end(), true);
    // Check if the game_master didn't change
    ASSERT_EQ(games->at("123").get_game_master(), player_1);
    ASSERT_EQ(games->at("123").get_full_game_state().get_players().size(), 1);
}

TEST(ServerLibraryTest, JoinLobby)
{
    server::MessageInterface message_interface;
    server::LobbyManager lobby_manager(message_interface);
    shared::PlayerBase::id_t player_1 = "Max";
    shared::PlayerBase::id_t player_2 = "Peter";
    shared::PlayerBase::id_t player_3 = "Paul";

    shared::CreateLobbyRequestMessage request1;
    request1.game_id = "123";
    request1.message_id = "456";
    request1.player_id = player_1;
    
    shared::JoinLobbyRequestMessage request2;
    request2.game_id = "123";
    request2.message_id = "789";
    request2.player_id = player_2;

    shared::JoinLobbyRequestMessage request3;
    request3.game_id = "222";
    request3.message_id = "101";
    request3.player_id = player_3;
    
    auto games = lobby_manager.get_games();
    
    ASSERT_EQ(games->empty(), true) << "LobbyManager should be empty at the beginning";
    
    lobby_manager.create_lobby(request1);
    ASSERT_EQ(games->size(), 1) << "LobbyManager should contain one lobby after creating one";
    ASSERT_EQ(games->find("123") != games->end(), true) << "Lobby with id 123 should exist";
    ASSERT_EQ(games->at("123").get_game_master(), player_1) << "Game master should be player_1";
    ASSERT_EQ(games->at("123").get_full_game_state().get_players().size(), 1) << "There should be one player in the lobby";
    
    lobby_manager.join_lobby(request2);
    ASSERT_EQ(games->at("123").get_full_game_state().get_players().size(), 2) << "There should be two players in the lobby";
    ASSERT_EQ(games->at("123").get_full_game_state().get_players().at(1).getId(), player_2) << "Player 2 should be in the lobby";
    
    // Player 2 should not be added again
    lobby_manager.join_lobby(request2);
    ASSERT_EQ(games->at("123").get_full_game_state().get_players().size(), 2) << "There should still be two players in the lobby";

    // Player 3 should not be able to join the lobby with id 123
    lobby_manager.join_lobby(request3);
    ASSERT_EQ(games->at("123").get_full_game_state().get_players().size(), 2) << "There should still be two players in the lobby";
    
}

TEST(ServerLibraryTest, StartGame)
{

}



// Test for the LobbyManager::join_lobby method => are players added to the lobby?