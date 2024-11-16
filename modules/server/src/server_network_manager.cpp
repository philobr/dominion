#include <iostream>
#include <sstream>

#include <server/network/server_network_manager.h>

using handler = std::function<void(const std::string &, const sockpp::tcp_socket::addr_t &)>;

namespace server
{
    const std::string DEFAULT_SERVER_HOST = "127.0.0.1";
    const unsigned int DEFAULT_PORT = 50505;

    BasicNetwork *BasicNetwork::_instance = nullptr;
    std::shared_ptr<MessageInterface> ServerNetworkManager::_messageInterface;
    LobbyManager ServerNetworkManager::_lobby_manager(ServerNetworkManager::_messageInterface);
    std::unique_ptr<MessageHandler> ServerNetworkManager::_messageHandler;

    ServerNetworkManager::ServerNetworkManager()
    {
        if ( _instance == nullptr ) {
            _instance = this;
        }
        _messageInterface = std::make_shared<MessageInterface>();
        _lobby_manager = LobbyManager(_messageInterface);
        _messageHandler = std::make_unique<MessageHandler>(MessageHandler(_lobby_manager));
        sockpp::socket_initializer socket_initializer; // Required to initialise sockpp
        this->connect(DEFAULT_SERVER_HOST, DEFAULT_PORT);
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
        listener_loop();
        // start endless loop
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
                std::string address = sock.peer_address().to_string();
                BasicNetwork::getInstance()->add_address_to_socket(address, sock.clone());
                // Create a listener thread and transfer the new stream to it.
                // Incoming messages will be passed to handle_message().
                std::thread listener(read_loop, std::move(sock), handle_message);

                listener.detach();
            }
        }
    }

    // Runs in a thread and reads anything coming in on the 'socket'.
    // Once a message is fully received, the string is passed on to the 'handle_message()' function
    void ServerNetworkManager::read_loop(sockpp::tcp_socket socket, const handler &message_handler)
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
                while ( i < count && buffer[i] != ':' ) {
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

        std::cout << "Read error [" << socket.last_error() << "]: " << socket.last_error_str() << std::endl;

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
            std::string address = peer_address.to_string();
            BasicNetwork::getInstance()->add_player_to_address(player_id, address);
            std::cerr << "Received a message" << std::endl;
#ifdef PRINT_NETWORK_MESSAGES
            std::cout << "Received valid request : " << msg << std::endl;
#endif
            // execute client request
            // TODO Change to message handler
            _messageHandler->HandleMessage(std::move(req));
            std::cerr << "Handled Message" << std::endl;

        } catch ( const std::exception &e ) {
            std::cerr << "Failed to execute client request. Content was :\n"
                      << msg << std::endl
                      << "Error was " << e.what() << std::endl;
        }
    }

    ssize_t ServerNetworkManager::send_message(std::unique_ptr<shared::ServerToClientMessage> message,
                                               const shared::PlayerBase::id_t &player_id)
    {
        std::string address = BasicNetwork::getInstance()->get_address(player_id);
        std::string msg = message->to_json();

        return BasicNetwork::getInstance()->send_message(msg, address);
    }

} // namespace server
