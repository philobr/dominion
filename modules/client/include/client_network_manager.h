#pragma once

#include <cstdint>
#include <string>
#include "client_listener.h"
#include "shared/message_types.h"
#include "sockpp/tcp_connector.h"

class ClientNetworkManager
{

public:
    static void init(const std::string &host, const uint16_t port);

    static void sendRequest(std::unique_ptr<shared::ClientToServerMessage> req);

    static void receiveMessage(const std::string &message);

    static void shutdown();

    static bool failedToConnect();

private:
    static bool connect(const std::string &host, const uint16_t port);


    static sockpp::tcp_connector *_connection;
    static ClientListener *_listener;

    static bool _connection_success;
    static bool _failed_to_connect;
};
