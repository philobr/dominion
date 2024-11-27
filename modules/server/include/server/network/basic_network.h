#pragma once

#include <shared_mutex>
#include <sstream>
#include <string>
#include <unordered_map>

#include <sockpp/tcp_socket.h>

using addr_t = sockpp::tcp_socket::addr_t;
using player_id_t = std::string;

namespace server
{
    class BasicNetwork
    {
        inline static std::unordered_map<player_id_t, std::string> _player_id_to_address;
        inline static std::unordered_map<std::string, sockpp::tcp_socket> _address_to_socket;

        inline static std::shared_mutex _rw_lock;

    public:
        // TODO:
        static void playerDisconnect(const player_id_t &player_id);

        /**
         * @brief Sends a message to the specified address.
         *
         * @param address
         * @param socket
         */
        static ssize_t sendToAddress(const std::string &message, const std::string &address);

        /**
         * @brief Sends a message to the specified player_id.
         *
         * @param address
         * @param socket
         */
        static ssize_t sendToPlayer(const std::string &message, const player_id_t &player_id);

        /**
         * @brief Maps a player ID to a network address.
         *
         * @param address
         * @param socket
         */
        static void addPlayerToAddress(const player_id_t &player_id, const std::string &address);

        /**
         * @brief Maps a network address to a socket.
         *
         * @param address
         * @param socket
         */
        static void addAddressToSocket(const std::string &address, sockpp::tcp_socket socket);

    private:
               static std::string getAddress(const player_id_t &player_id);
        static sockpp::tcp_socket *getSocket(const std::string &address);
        static bool isNewPlayer(const player_id_t &player_id);
    };
} // namespace server