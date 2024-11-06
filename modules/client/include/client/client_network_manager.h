#include <string>
#include "client/client_listener.h"
#include "shared/message_types.h"

class ClientNetworkManager {

public:
    static void init(const std::string& host, const uint16_t port);

    static void sendRequest(const std::unique_ptr<shared::ClientToServerMessage>& request);

    static void receive_message(const std::string& message);

private:
    static bool connect(const std::string& host, const uint16_t port);


    static sockpp::tcp_connector* _connection;

    static bool _connectionSuccess;
    static bool _failedToConnect;

};