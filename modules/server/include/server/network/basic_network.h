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
        ssize_t send_message(std::string &message, std::string &address);

        void add_player_to_address(player_id_t &player_id, std::string &address);

        void add_address_to_socket(std::string &address, sockpp::tcp_socket socket);

        bool is_new_player(player_id_t &player_id);

        std::string get_address(const player_id_t &player_id);

        void player_disconnect(std::string player_id);

        static BasicNetwork *getInstance();

    private:
        std::unordered_map<std::string, std::string> _player_id_to_address;
        std::unordered_map<std::string, sockpp::tcp_socket> _address_to_socket;

        std::shared_mutex _rw_lock;

        static BasicNetwork *_instance;
    };
} // namespace server