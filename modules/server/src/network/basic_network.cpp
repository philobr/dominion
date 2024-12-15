#include <server/network/basic_network.h>
#include <shared/utils/logger.h>
#include <string>
#include "server/network/server_network_manager.h"
#include "shared/message_types.h"

namespace server
{
    using shared::ResultResponseMessage;

    ssize_t BasicNetwork::sendToAddress(const std::string &message, const std::string &address)
    {
        LOG(INFO) << "Sending Message: " << message << " to Address: " << address;
        try {
            sockpp::tcp_socket *socket;

            {
                std::shared_lock<std::shared_mutex> lock(_rw_lock);
                socket = getSocket(address);
            }

            if ( socket == nullptr ) {
                LOG(ERROR) << "Failed to get socket for address: " << address;
                return ssize_t(-1);
            }

            std::stringstream ss_msg;
            ss_msg << std::to_string(message.size()) << ':' << message; // prepend message length
            sockpp::result<size_t> res = socket->write(ss_msg.str()); // TODO: make this thread safe (wrapper class)
            if ( res.is_error() ) {
                LOG(ERROR) << "Failed to send message to address: " << address
                           << ". Socket error: " << res.error_message();
            } else {
                LOG(INFO) << "Successfully sent Message: " << message;
            }

            return ssize_t(res.value());
        } catch ( const std::runtime_error &e ) {
            LOG(ERROR) << "Error in sendMessage: " << e.what();
            return ssize_t(-1); // indicate failure
        }
    }

    ssize_t BasicNetwork::sendToPlayer(const std::string &message, const player_id_t &player_id)
    {
        std::string address;

        {
            std::shared_lock<std::shared_mutex> lock(_rw_lock);
            address = getAddress(player_id);
        }

        return sendToAddress(message, address);
    }

    bool BasicNetwork::addPlayerToAddress(const player_id_t &player_id, const std::string &lobby_id,
                                          const std::string &address)
    {
        // check with shared lock
        {
            std::shared_lock<std::shared_mutex> shared_lock(_rw_lock);
            if ( !isNewPlayer(player_id) ) {
                LOG(INFO) << "Player with ID " << player_id << " is already registered.";
                if ( getAddress(player_id) != address ) {
                    // There is already a player with this name
                    shared::ResultResponseMessage msg = shared::ResultResponseMessage(
                            "No lobby", false, "in_response_to deprecated", "This name is already taken!");
                    sendToAddress(msg.toJson(), address);
                    return false;
                }
                return true;
            }
        }

        // acquire unique lock and re-check
        {
            std::unique_lock<std::shared_mutex> lock(_rw_lock);
            if ( !isNewPlayer(player_id) ) {
                LOG(WARN) << "Player with ID " << player_id << " is already registered (after lock).";
                if ( getAddress(player_id) != address ) {
                    // There is already a player with this name
                    shared::ResultResponseMessage msg = shared::ResultResponseMessage(
                            "No lobby", false, "in_response_to deprecated", "This name is already taken!");
                    sendToAddress(msg.toJson(), address);
                    return false;
                }
                return true;
            }
            LOG(INFO) << "Registering new client with ID: " << player_id;
            _player_id_to_address.emplace(player_id, address);
            _address_to_player_id.emplace(address, player_id);
            _player_id_to_lobby_id.emplace(player_id, lobby_id);
        }
        return true;
    }

    void BasicNetwork::addAddressToSocket(const std::string &address, const sockpp::tcp_socket socket)
    {
        std::unique_lock<std::shared_mutex> lock(_rw_lock);

        if ( _address_to_socket.count(address) != 0 ) {
            LOG(ERROR) << "Address is already connected: " << address;
        } else {
            LOG(DEBUG) << "Adding address: " << address;
            _address_to_socket.emplace(address, socket.clone());
        }
    }

    void BasicNetwork::playerDisconnect(const std::string &address)
    {
        LOG(INFO) << "Disconnecting Address: " << address;
        std::unique_lock<std::shared_mutex> lock(_rw_lock);

        if ( _address_to_player_id.find(address) != _address_to_player_id.end() ) {
            player_id_t player_id = _address_to_player_id.find(address)->second;
            lock.unlock();
            ServerNetworkManager::removePlayer(_player_id_to_lobby_id.find(player_id)->second, player_id);
            lock.lock();
            _player_id_to_lobby_id.erase(player_id);
            _player_id_to_address.erase(player_id);
            _address_to_player_id.erase(address);
            _address_to_socket.erase(address);
            LOG(INFO) << "Player with Address " << address << " disconnected and resources released.";
        } else {
            LOG(WARN) << "Attempted to disconnect player with Address " << address << ", but it was not found.";
        }
    }


    // ================================================================
    // LOCKS FOR FUNCTIONS BELOW MUST BE ACCUIRED BY CALLER
    // ================================================================

    bool BasicNetwork::isNewPlayer(const player_id_t &player_id)
    {
        // ASSUMING CALLER HOLDS THE LOCK!
        return _player_id_to_address.find(player_id) == _player_id_to_address.end();
    }

    sockpp::tcp_socket *BasicNetwork::getSocket(const std::string &address)
    {
        // ASSUMING CALLER HOLDS THE LOCK!
        auto it = _address_to_socket.find(address);
        if ( it != _address_to_socket.end() ) {
            return &(it->second);
        } else {
            LOG(ERROR) << "Cannot find socket for address: " << address;
            return nullptr;
        }
    }

    const std::string &BasicNetwork::getAddress(const player_id_t &player_id)
    {
        // ASSUMING CALLER HOLDS THE LOCK!
        auto it = _player_id_to_address.find(player_id);
        if ( it != _player_id_to_address.end() ) {
            return it->second;
        } else {
            LOG(ERROR) << "Cannot find address for player ID: " << player_id;
            throw std::runtime_error("Invalid player_id: " + player_id);
        }
    }
} // namespace server
