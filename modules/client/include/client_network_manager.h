#pragma once

#include <cstdint>
#include <string>
#include "sockpp/tcp_connector.h"

class ClientNetworkManager
{

public:
    static void init(const std::string &host, const uint16_t port);

    static void sendRequest(const std::string message);

    static void receiveMessage(const std::string &message);

    static void shutdown();

private:
    static bool connect(const std::string &host, const uint16_t port);


    static sockpp::tcp_connector *_connection;

    static bool _connectionSuccess;
    static bool _failedToConnect;
};
