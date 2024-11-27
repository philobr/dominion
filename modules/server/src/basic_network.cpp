#include <server/network/basic_network.h>
#include <shared/utils/logger.h>

namespace server
{

    ssize_t BasicNetwork::sendMessage(const std::string &message, const std::string &address)
    {

        _rw_lock.lock();

        LOG(INFO) << "Sending Message: " << message << " to Address: " << address;
        std::stringstream ss_msg;
        ss_msg << std::to_string(message.size()) << ':' << message; // prepend message length
        ssize_t ret = _address_to_socket.at(address).write(ss_msg.str());

        _rw_lock.unlock();
        return ret;
    }

    void BasicNetwork::addPlayerToAddress(const player_id_t &player_id, const std::string &address)
    {
        _rw_lock.lock_shared();
        if ( isNewPlayer(player_id) ) {
            // save connection to this client
            _rw_lock.unlock_shared();
            LOG(INFO) << "New client with id " << player_id;
            _rw_lock.lock();
            _player_id_to_address.emplace(player_id, address);
            _rw_lock.unlock();
        }
    }

    void BasicNetwork::addAddressToSocket(const std::string &address, const sockpp::tcp_socket socket)
    {
        _rw_lock.lock();
        _address_to_socket.emplace(address, socket.clone());
        _rw_lock.unlock();
    }

    bool BasicNetwork::isNewPlayer(const player_id_t &player_id)
    {
        return _player_id_to_address.find(player_id) == _player_id_to_address.end();
    }

    std::string BasicNetwork::getAddress(const player_id_t &player_id)
    {
        if ( _player_id_to_address.find(player_id) != _player_id_to_address.end() ) {
            return _player_id_to_address[player_id];
        }
        return "";
    }

    void BasicNetwork::playerDisconnect(const std::string player_id)
    {
        _rw_lock.lock();
        std::string address = _player_id_to_address[player_id];
        _player_id_to_address.erase(player_id);
        _address_to_socket.erase(address);
        _rw_lock.unlock();
    }

    BasicNetwork *BasicNetwork::getInstance()
    {
        if ( _instance == nullptr ) {
            _instance = new BasicNetwork();
        }
        return _instance;
    }
} // namespace server
