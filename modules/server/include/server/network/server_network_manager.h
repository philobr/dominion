#pragma once

#include <functional>
#include <shared_mutex>
#include <string>
#include <thread>
#include <unordered_map>


#include "rapidjson/document.h"
#include "sockpp/tcp_acceptor.h"
#include "sockpp/tcp_socket.h"

#include <server/lobbies/lobby_manager.h>
#include <server/network/basic_network.h>
#include <server/network/message_handler.h>
#include <server/network/message_interface.h>
#include <shared/message_types.h>

using handler = std::function<void(const std::string &, const sockpp::tcp_socket::addr_t &)>;

namespace server
{
    const std::string DEFAULT_SERVER_HOST = "127.0.0.1";
    const uint16_t DEFAULT_PORT = 50505;

    class ServerNetworkManager
    {
    public:
        ServerNetworkManager(uint16_t port = DEFAULT_PORT);
        ~ServerNetworkManager();

        // function to send via the BasicNetwork class
        static ssize_t send_message(std::unique_ptr<shared::ServerToClientMessage> message,
                                    const shared::PlayerBase::id_t &player_id);

    private:
        // Lobby object to pass received messages to
        static LobbyManager _lobby_manager;

        // Might get removed later, tbd
        inline static ServerNetworkManager *_instance;

        inline static std::shared_mutex _rw_lock;
        inline static sockpp::tcp_acceptor _acc;
        // messageHandler does the logic for received messages
        static std::unique_ptr<MessageHandler> _messageHandler;
        // message interface gets passes to lobby manager etc. for the to send to clients later
        static std::shared_ptr<MessageInterface> _messageInterface;

        // connect new clients
        void connect(const std::string &url, const uint16_t port);

        // function that listens to new clients
        static void listener_loop();
        static void read_loop(sockpp::tcp_socket socket, const handler &message_handler);

        // might get removed later
        static void handle_message(const std::string &, const sockpp::tcp_socket::addr_t &);
    };
} // namespace server
