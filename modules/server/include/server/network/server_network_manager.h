#pragma once

#include <functional>
#include <shared_mutex>
#include <string>
#include <thread>
#include <unordered_map>


#include "rapidjson/document.h"
#include "sockpp/tcp_acceptor.h"
#include "sockpp/tcp_socket.h"

#include <server/network/message_handler.h>
#include <server/lobbies/lobby_manager.h>
#include <shared/message_types.h>
#include <server/network/basic_network.h>
#include <server/network/message_interface.h>

namespace server
{
    class ServerNetworkManager
    {
    public:
        ServerNetworkManager();
        ~ServerNetworkManager();

        static ssize_t send_message(std::unique_ptr<shared::ServerToClientMessage> message,
                                    shared::PlayerBase::id_t &player_id);

    private:
        static LobbyManager _lobby_manager;

        inline static ServerNetworkManager *_instance;
        inline static std::shared_mutex _rw_lock;
        inline static sockpp::tcp_acceptor _acc;
        static std::unique_ptr<MessageHandler> _messageHandler;
        static std::shared_ptr<MessageInterface> _messageInterface;

        static std::shared_ptr<BasicNetwork> basic_network;

        void connect(const std::string &url, const uint16_t port);

        static void listener_loop();
        static void
        read_loop(sockpp::tcp_socket socket,
                  const std::function<void(const std::string &, const sockpp::tcp_socket::addr_t &)> &message_handler);

        static void handle_message(const std::string &, const sockpp::tcp_socket::addr_t &);
    };
} // namespace server