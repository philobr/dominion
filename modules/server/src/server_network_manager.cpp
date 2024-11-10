
#include <iostream>
#include <sstream>

#include "server/server_network_manager.h"

#include <shared/default.conf>
#include "shared/message_types.h"


namespace server
{

    ServerNetworkManager::ServerNetworkManager()
    {
        if ( _instance == nullptr ) {
            _instance = this;
        }
        sockpp::socket_initializer socket_initializer; // Required to initialise sockpp
        this->connect(default_server_host, default_port); // variables from "default.conf"
    }

    ServerNetworkManager::~ServerNetworkManager() = default;

    void ServerNetworkManager::connect(const std::string &url, const uint16_t port)
    {
        this->_acc = sockpp::tcp_acceptor(port);

        if ( !_acc ) {
            std::cerr << "Error creating the acceptor: " << _acc.last_error_str() << std::endl;
            return;
        }

        std::cout << "Awaiting connections on port " << port << "..." << std::endl;
        listener_loop(); // start endless loop
    }

    void ServerNetworkManager::listener_loop()
    {
        // intentional endless loop
        while ( true ) {
            sockpp::inet_address peer;

            // Accept a new client connection
            sockpp::tcp_socket sock = _acc.accept(&peer);
            std::cout << "Received a connection request from " << peer << std::endl;

            if ( !sock ) {
                std::cerr << "Error accepting incoming connection: " << _acc.last_error_str() << std::endl;
            } else {
                _rw_lock.lock();
                _address_to_socket.emplace(sock.peer_address().to_string(), std::move(sock.clone()));
                _rw_lock.unlock();
                // Create a listener thread and transfer the new stream to it.
                // Incoming messages will be passed to handle_message().
                std::thread listener(read_loop, std::move(sock), handle_message);

                listener.detach();
            }
        }
    }

    // Runs in a thread and reads anything coming in on the 'socket'.
    // Once a message is fully received, the string is passed on to the 'handle_message()' function
    void ServerNetworkManager::read_loop(
            sockpp::tcp_socket socket,
            const std::function<void(const std::string &, const sockpp::tcp_socket::addr_t &)> &message_handler)
    {
        sockpp::socket_initializer sockInit; // initializes socket framework underneath

        char buffer[512]; // 512 bytes
        ssize_t count = 0;
        ssize_t msg_bytes_read = 0;
        ssize_t msg_length = 0;

        while ( (count = socket.read(buffer, sizeof(buffer))) > 0 ) {
            try {
                int i = 0;
                std::stringstream ss_msg_length;
                while ( buffer[i] != ':' && i < count ) {
                    ss_msg_length << buffer[i];
                    i++;
                }
                msg_length = std::stoi(ss_msg_length.str());
                std::cout << "Expecting message of length " << msg_length << std::endl;

                // put everything after the message length declaration into a stringstream
                std::stringstream ss_msg;
                ss_msg.write(&buffer[i + 1], count - (i + 1));
                msg_bytes_read = count - (i + 1);

                // read the remaining packages
                while ( msg_bytes_read < msg_length && count > 0 ) {
                    count = socket.read(buffer, sizeof(buffer));
                    msg_bytes_read += count;
                    ss_msg.write(buffer, count);
                }

                if ( msg_bytes_read == msg_length ) {
                    // sanity check that really all bytes got read (possibility that count was <= 0, indicating a read
                    // error)
                    std::string msg = ss_msg.str();
                    message_handler(msg, socket.peer_address()); // attempt to parse client_request from 'msg'
                } else {
                    std::cerr << "Could not read entire message. TCP stream ended before. Difference is "
                              << msg_length - msg_bytes_read << std::endl;
                }
            } catch ( std::exception &e ) { // Make sure the connection isn't torn down only because of a read error
                std::cerr << "Error while reading message from " << socket.peer_address() << std::endl
                          << e.what() << std::endl;
            }
        }
        if ( count <= 0 ) {
            std::cout << "Read error [" << socket.last_error() << "]: " << socket.last_error_str() << std::endl;
        }

        std::cout << "Closing connection to " << socket.peer_address() << std::endl;
        socket.shutdown();
    }


    void ServerNetworkManager::handle_message(const std::string &msg, const sockpp::tcp_socket::addr_t &peer_address)
    {
        try {
            // try to parse a client_request from msg
            std::unique_ptr<shared::ClientToServerMessage> req = shared::ClientToServerMessage::from_json(msg);

            if ( req == nullptr ) {
                // TODO: handle invalid message
                throw std::runtime_error("Not implemented yet");
            }

            // check if this is a connection to a new player
            shared::PlayerBase::id_t player_id = req->player_id;
            _rw_lock.lock_shared();
            if ( _player_id_to_address.find(player_id) == _player_id_to_address.end() ) {
                // save connection to this client
                _rw_lock.unlock_shared();
                std::cout << "New client with id " << player_id << std::endl;
                _rw_lock.lock();
                _player_id_to_address.emplace(player_id, peer_address.to_string());
                _rw_lock.unlock();
            } else {
                _rw_lock.unlock_shared();
            }
#ifdef PRINT_NETWORK_MESSAGES
            std::cout << "Received valid request : " << msg << std::endl;
#endif
            // execute client request
            MessageInterface::handle_request(std::move(req));

        } catch ( const std::exception &e ) {
            std::cerr << "Failed to execute client request. Content was :\n"
                      << msg << std::endl
                      << "Error was " << e.what() << std::endl;
        }
    }


    void ServerNetworkManager::player_disconnect(std::string player_id)
    {
        _rw_lock.lock();
        std::string address = _player_id_to_address[player_id];
        _player_id_to_address.erase(player_id);
        _address_to_socket.erase(address);
        _rw_lock.unlock();
    }


    ssize_t ServerNetworkManager::send_message(const std::string &msg, const std::string &address)
    {
        std::stringstream ss_msg;
        ss_msg << std::to_string(msg.size()) << ':' << msg; // prepend message length
        return _address_to_socket.at(address).write(ss_msg.str());
    }

    void ServerNetworkManager::broadcast_message(const std::string &msg, const std::string &address,
                                                 const std::vector<shared::PlayerBase::id_t> &players,
                                                 const shared::PlayerBase::id_t exclude)
    {
#ifdef PRINT_NETWORK_MESSAGES
        std::cout << "Broadcasting message : " << msg_string << std::endl;
#endif

        _rw_lock.lock_shared();
        // send object_diff to all requested players
        try {
            for ( auto &player_id : players ) {
                if ( player_id != exclude ) {
                    send_message(msg, _player_id_to_address.at(player_id));
                }
            }
        } catch ( std::exception &e ) {
            std::cerr << "Encountered error when sending state update: " << e.what() << std::endl;
        }
        _rw_lock.unlock_shared();
    }

} // namespace server
