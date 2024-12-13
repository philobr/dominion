#include <client_network_manager.h>
#include "dominion.h"

#include <client_listener.h>
#include <game_controller.h>

#include <shared/message_types.h>

#include <shared/utils/logger.h>
#include <sockpp/exception.h>
#include <sstream>

// initialize static members
sockpp::tcp_connector *ClientNetworkManager::_connection = nullptr;
ClientListener *ClientNetworkManager::_listener = nullptr;

bool ClientNetworkManager::_connection_success = false;
bool ClientNetworkManager::_failed_to_connect = false;


void ::ClientNetworkManager::init(const std::string &host, const uint16_t port)
{
    LOG(INFO) << "Initializing ClientNetworkManager";
    // initialize sockpp framework
    sockpp::socket_initializer sockInit;

    // reset connection status
    ClientNetworkManager::_connection_success = false;
    ClientNetworkManager::_failed_to_connect = false;

    // delete exiting connection and create new one
    if ( ClientNetworkManager::_connection != nullptr ) {
        if ( ClientNetworkManager::_listener != nullptr ) {
            ClientNetworkManager::_listener->shutdown();
            ClientNetworkManager::_listener = nullptr;
        }
        ClientNetworkManager::_connection->shutdown();
        delete ClientNetworkManager::_connection;
        LOG(INFO) << "Removed old connection";
    }
    ClientNetworkManager::_connection = new sockpp::tcp_connector();

    // try to connect to server
    if ( ClientNetworkManager::connect(host, port) ) {
        LOG(INFO) << "Connected to " << host << ":" << std::to_string(port);
        wxGetApp().getController().showStatus("Connected to " + host + ":" + std::to_string(port));
        ClientNetworkManager::_connection_success = true;
        // start network thread
        ClientNetworkManager::_listener = new ClientListener(ClientNetworkManager::_connection);

        if ( ClientNetworkManager::_listener->Run() != wxTHREAD_NO_ERROR ) {
            LOG(ERROR) << "Could not create client network thread";
            wxGetApp().getController().showError("Connection error", "Could not create client network thread");
        }

    } else {
        ClientNetworkManager::_failed_to_connect = true;
        LOG(ERROR) << "Failed to connect";
        wxGetApp().getController().showStatus("Not connected");
    }
}


bool ClientNetworkManager::connect(const std::string &host, const uint16_t port)
{
    try {

        // create sockpp address and catch any errors
        sockpp::inet_address address = sockpp::inet_address(host, port);

        // establish connection to given address
        if ( !ClientNetworkManager::_connection->connect(address) ) {
            wxGetApp().getController().showError("Connection error",
                                                 "Failed to connect to server " + address.to_string());
            LOG(ERROR) << "Failed to connect to server " << address.to_string();
            return false;
        }
    } catch ( const sockpp::getaddrinfo_error &e ) {
        wxGetApp().getController().showError("Connection error", "Failed to resolve address " + e.hostname());
        LOG(ERROR) << "Failed to resolve address" << e.hostname();
        return false;
    }

    return true;
}


void ClientNetworkManager::sendRequest(std::unique_ptr<shared::ClientToServerMessage> req)
{
    // wait until network is connected (max. 5 seconds)
    int connectionCheckCounter = 0;
    while ( !ClientNetworkManager::_connection_success && !ClientNetworkManager::_failed_to_connect &&
            connectionCheckCounter < 200 ) {
        wxMilliSleep(25);
        connectionCheckCounter++;
    }

    // do not continue if failed to connect to server
    if ( ClientNetworkManager::_failed_to_connect ) {
        LOG(ERROR) << "Failed to connect to server";
        return;
    }

    if ( ClientNetworkManager::_connection_success && ClientNetworkManager::_connection->is_connected() ) {
        LOG(INFO) << "Connected to server";

        // convert message to json
        std::string message = req->toJson();

        // turn message into stream and prepend message length
        std::stringstream messageStream;
        messageStream << std::to_string(message.size()) << ':' << message;
        std::string msg = messageStream.str();

        // output message for debugging purposes
        LOG(INFO) << "Sending request : " << msg;
        // send message to server
        ssize_t bytesSent = ClientNetworkManager::_connection->write(msg);

        // if the number of bytes sent does not match the length of the msg, probably something went wrong
        if ( bytesSent != ssize_t(msg.length()) ) {
            LOG(ERROR) << "Error writing to TCP stream: " << ClientNetworkManager::_connection->last_error_str();
            wxGetApp().getController().showError("Network error",
                                                 "Error writing to the TCP stream: " +
                                                         ClientNetworkManager::_connection->last_error_str());
        }

    } else {
        LOG(ERROR) << "Lost connection to server";
        wxGetApp().getController().showError("Network error", "Lost connection to server");
    }
}


void ClientNetworkManager::receiveMessage(const std::string &message)
{
    try {
        std::unique_ptr<shared::ServerToClientMessage> res = shared::ServerToClientMessage::fromJson(message);
        LOG(INFO) << "Received Message: " << message;
        wxGetApp().getController().receiveMessage(std::move(res));
    } catch ( std::exception &e ) {
        LOG(ERROR) << "Exception in ClientNetworkManager::receive_message: " << e.what();
        wxGetApp().getController().showError("JSON parsing error",
                                             "Failed to parse message from server:\n" + message + "\n" +
                                                     (std::string)e.what());
    }
}

void ClientNetworkManager::shutdown() { ClientNetworkManager::_connection->shutdown(); }

bool ClientNetworkManager::failedToConnect()
{
    return !(ClientNetworkManager::_connection_success && ClientNetworkManager::_connection->is_connected());
}
