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
    public:
        ssize_t sendMessage(const std::string &message, const std::string &address);
        static BasicNetwork *getInstance();


        void addPlayerToAddress(const player_id_t &player_id, const std::string &address);

        void addAddressToSocket(const std::string &address, sockpp::tcp_socket socket);

        bool isNewPlayer(const player_id_t &player_id);

        std::string getAddress(const player_id_t &player_id);
        sockpp::tcp_socket *getSocket(const std::string &address);

        void playerDisconnect(const player_id_t &player_id);

    private:
        std::unordered_map<std::string, std::string> _player_id_to_address;
        std::unordered_map<std::string, sockpp::tcp_socket> _address_to_socket;

        std::shared_mutex _rw_lock;
    };
} // namespace server