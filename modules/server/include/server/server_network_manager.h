#pragma once

#include <functional>
#include <shared_mutex>
#include <string>
#include <thread>
#include <unordered_map>


#include "rapidjson/document.h"
#include "sockpp/tcp_acceptor.h"
#include "sockpp/tcp_socket.h"

#include <shared/message_types.h>
#include <server/lobbies.h>

namespace server
{
    class ServerNetworkManager
    {
    public:
        ServerNetworkManager();
        ~ServerNetworkManager();

        static ssize_t send_message(std::unique_ptr<shared::ServerToClientMessage> message, const shared::PlayerBase::id_t& player_id);

        static void player_disconnect(std::string player_id);

        // MessageInterface get_message_interface();

    private:
        // LobbyManager lobby_manager;

        inline static ServerNetworkManager *_instance;
        inline static std::shared_mutex _rw_lock;
        inline static sockpp::tcp_acceptor _acc;
//        static MessageInterface *_messageInterface; //TODO swap for message handler

        inline static std::unordered_map<std::string, std::string> _player_id_to_address;
        inline static std::unordered_map<std::string, sockpp::tcp_socket> _address_to_socket;

        void connect(const std::string &url, const uint16_t port);

        static void listener_loop();
        static void
        read_loop(sockpp::tcp_socket socket,
                  const std::function<void(const std::string &, const sockpp::tcp_socket::addr_t &)> &message_handler);

        static void handle_message(const std::string &, const sockpp::tcp_socket::addr_t &);
    };

    /**
     * @brief Abstract base class for sending messages to the client
     *
     * @details Used for enabling Mocking of the send_message method
     * for writing tests.
     */
    class MessageInterface
    {
    public:
        
        static void init();
        /**
         * @brief gets a response that has to be sent to a client and passes it to the network manager as json string
         */
        // TODO: I think this also needs to take in a player ID
        static void send_message(std::unique_ptr<shared::ServerToClientMessage> message, const shared::PlayerBase::id_t& player_id);

    private:
    static std::unique_ptr<ServerNetworkManager> _network_manager;
    };
} // namespace server