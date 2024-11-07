#include <server/lobbies.h>
#include <server/message_interface.h>
#include <shared/game_state.h>
#include <shared/message_types.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::Return;
using ::testing::Invoke;
using ::testing::Truly;


namespace server {
    // Mock class for MessageInterface
    class MockMessageInterface : public MessageInterface {
    public:
        // Mock the send_message method; this doesn't require a definition in MessageInterface.
        MOCK_METHOD(void, send_message, (shared::ResultResponseMessage *message, shared::PlayerBase::id_t player_id), (override));
    };
}

TEST(ServerLibraryTest, CreateLobby)
{
    server::MockMessageInterface mock_message_interface;
    server::LobbyManager lobby_manager(mock_message_interface);
    shared::PlayerBase::id_t player_1 = "Max";
    shared::PlayerBase::id_t player_2 = "Peter";

    shared::CreateLobbyRequestMessage request1;
    request1.game_id = "123";
    request1.message_id = "456";
    request1.player_id = player_1;

    ASSERT_EQ(request1.game_id, "faesd");

    EXPECT_CALL(mock_message_interface, send_message(
        Truly([&](shared::ServerToClientMessage *msg) {
            auto result_msg = dynamic_cast<shared::ResultResponseMessage *>(msg);
            return result_msg && result_msg->is_success() == true && result_msg->message_id == request1.message_id;
        }),
        player_1
    )).Times(2);

    lobby_manager.create_lobby(request1);
    
    

    
    //server::LobbyBase::id_t lobby_id = lobby_manager.create_lobby("player1");
    //ASSERT_EQ(lobby_manager.get_lobby(lobby_id)->get_player_ids().size(), 1);
    //ASSERT_EQ(lobby_manager.get_lobby(lobby_id)->get_player_ids()[0], "player1");
    
}





// Test for the LobbyManager::join_lobby method => are players added to the lobby?