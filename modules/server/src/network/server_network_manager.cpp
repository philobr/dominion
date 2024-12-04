
#include <iostream>
#include <sstream>

#include <server/network/server_network_manager.h>
#include <shared/utils/logger.h>

using handler = std::function<void(const std::string &, const sockpp::tcp_socket::addr_t &)>;

namespace server
{
    std::shared_ptr<MessageInterface> ServerNetworkManager::_message_interface;
    LobbyManager ServerNetworkManager::_lobby_manager(ServerNetworkManager::_message_interface);

    ServerNetworkManager::ServerNetworkManager()
    {
        // @matthieu, should this be singleton?
        if ( _instance == nullptr ) {
            _instance = this;
        }
        _message_interface = std::make_shared<ImplementedMessageInterface>();
        _lobby_manager = LobbyManager(_message_interface);
    }

    void ServerNetworkManager::run(const std::string &host, uint16_t port)
    {
        LOG(INFO) << "Running the server on " << host << ":" << port;
        sockpp::socket_initializer socket_initializer; // Required to initialise sockpp
        this->connect(host, port);
    }

    ServerNetworkManager::~ServerNetworkManager() = default;

    // TODO: Why does this pass a URL when it is never used?
    void ServerNetworkManager::connect(const std::string & /*url*/, const uint16_t port)
    {
        this->_acc = sockpp::tcp_acceptor(port);

        if ( !_acc ) {
            LOG(ERROR) << "Error creating the acceptor: " << _acc.last_error_str();
            return;
        }

        LOG(INFO) << "Awaiting connections on port " << port;
        listenerLoop();
    }

    void ServerNetworkManager::listenerLoop()
    {
        LOG(INFO) << "Starting a new listener loop";
        // intentional endless loop
        while ( true ) {
            sockpp::inet_address peer;

            // Accept a new client connection
            sockpp::tcp_socket sock = _acc.accept(&peer);
            LOG(DEBUG) << "Received a connection request from peer(" << peer << ")";

            if ( !sock ) {
                LOG(ERROR) << "Error accepting incoming connection: " << _acc.last_error_str();
                return;
            }

            const std::string address = sock.peer_address().to_string();
            BasicNetwork::addAddressToSocket(address, sock.clone());

            // Create a listener thread and transfer the new stream to it.
            // Incoming messages will be passed to handle_message().
            std::thread listener(readLoop, std::move(sock), handleMessage);
            listener.detach();
        }
    }

    // Runs in a thread and reads anything coming in on the 'socket'.
    // Once a message is fully received, the string is passed on to the 'handle_message()' function
    void ServerNetworkManager::readLoop(sockpp::tcp_socket socket, const handler &message_handler)
    {
        sockpp::socket_initializer sockInit; // initializes socket framework

        constexpr size_t BUFFER_SIZE = 512;
        std::string buffer(BUFFER_SIZE, '\0');
        ssize_t count = 0;

        while ( (count = socket.read(buffer.data(), buffer.size())) > 0 ) {
            try {
                std::string_view read_data(buffer.data(), count);
                size_t separator_pos = read_data.find(':');

                if ( separator_pos == std::string_view::npos ) {
                    LOG(ERROR) << "Malformed message: Missing length separator ':'";
                    continue;
                }

                size_t msg_length = std::stoul(std::string(read_data.substr(0, separator_pos)));
                LOG(INFO) << "Expecting message of length " << msg_length;

                // accumulate the message payload
                std::string message;
                message.reserve(msg_length);
                message.append(read_data.substr(separator_pos + 1));
                size_t msg_bytes_read = message.size();

                // read the remaining packages
                while ( msg_bytes_read < msg_length ) {
                    count = socket.read(buffer.data(), buffer.size());
                    if ( count <= 0 ) {
                        break; // end of stream or error
                    }

                    message.append(buffer.data(), count);
                    msg_bytes_read += count;
                }

                if ( msg_bytes_read == msg_length ) {
                    LOG(INFO) << "Received Message: " << message;
                    message_handler(message, socket.peer_address());
                } else {
                    LOG(ERROR) << "Incomplete message. Expected " << msg_length << " bytes, but received "
                               << msg_bytes_read;
                }
            } catch ( const std::exception &e ) {
                LOG(ERROR) << "Error while reading message from " << socket.peer_address() << ": " << e.what();
            }
        }

        if ( count < 0 ) {
            // negative count indicates error
            LOG(ERROR) << "Read error [" << socket.last_error() << "]: " << socket.last_error_str();
        }

        LOG(DEBUG) << "Closing connection to " << socket.peer_address();
        socket.shutdown();
    }

    void ServerNetworkManager::handleMessage(const std::string &msg, const sockpp::tcp_socket::addr_t &peer_address)
    {
        try {
            // try to parse a client_request from msg
            std::unique_ptr<shared::ClientToServerMessage> req = shared::ClientToServerMessage::fromJson(msg);

            if ( req == nullptr ) {
                // TODO: handle invalid message
                LOG(ERROR) << "Failed to parse message";
                throw std::runtime_error("Received an unparsable message");
            }

            // check if this is a connection to a new player
            BasicNetwork::addPlayerToAddress(req->player_id, peer_address.to_string());
            LOG(INFO) << "Handling request from player(" << req->player_id << "): " << msg;

            _lobby_manager.handleMessage(req);

        } catch ( const std::exception &e ) {
            LOG(ERROR) << FUNC_NAME << ": Failed to execute client request. Content was :\n"
                       << msg << std::endl
                       << "Error was " << e.what();
        }
    }

    ssize_t ServerNetworkManager::sendMessage(std::unique_ptr<shared::ServerToClientMessage> message,
                                              const shared::PlayerBase::id_t &player_id)
    {
        return BasicNetwork::sendToPlayer(message->toJson(), player_id);
    }

} // namespace server
