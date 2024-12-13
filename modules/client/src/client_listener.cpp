#include "client_listener.h"
#include <cerrno>
#include <cstddef>
#include <shared/utils/logger.h>
#include <unistd.h>
#include "client_network_manager.h"


ClientListener::ClientListener(sockpp::tcp_connector *connection) :
    wxThread(wxTHREAD_DETACHED), _connection(connection), _isActive(true)
{}


ClientListener::~ClientListener() = default;

void ClientListener::shutdown() { this->_isActive = false; }

wxThread::ExitCode ClientListener::Entry()
{
    try {
        char buffer[512]; // 512 bytes
        std::string leftover; // To store incomplete messages across reads
        ssize_t count = 0;

        this->_connection->set_non_blocking();

        while ( this->isActive() ) {
            try {
                count = this->_connection->read(buffer, sizeof(buffer));
                // if you get a message, read it
                if ( count > 0 ) {
                    // Append new data to leftover string
                    leftover.append(buffer, count);

                    while ( !leftover.empty() ) {

                        size_t colonPos = leftover.find(':');
                        if ( colonPos == std::string::npos ) {
                            // Wait for more data if ':' is not yet available
                            break;
                        }

                        try {
                            ssize_t messageLength = std::stoi(leftover.substr(0, colonPos));
                            size_t totalMessageSize = colonPos + 1 + messageLength;

                            // Check if the full message is available
                            if ( leftover.size() < totalMessageSize ) {
                                // Wait for more data
                                break;
                            }

                            std::string message = leftover.substr(colonPos + 1, messageLength);
                            leftover = leftover.substr(totalMessageSize);

                            ClientNetworkManager::receiveMessage(message);

                        } catch ( const std::exception &e ) {
                            LOG(ERROR) << "Network error. Error while reading message: " << e.what();
                            leftover.clear(); // Reset leftover to avoid infinite errors
                            break;
                        }
                    }
                } else if ( this->_connection->last_error() != EWOULDBLOCK ) {
                    // Connection Error
                    LOG(ERROR) << "Network error: Read error, shutting down Listener";
                    break;
                }

            } catch ( std::exception &e ) {
                // Make sure the connection isn't terminated only because of a read error
                LOG(ERROR) << "Network error: " << e.what();
            }
            // wxThread sleep function in milliseconds
            Sleep(5);
        }

        LOG(ERROR) << "Network error. Read error, shutting down Listener";

    } catch ( const std::exception &e ) {
        LOG(ERROR) << "Network error. Error in listener thread: " << std::string(e.what());
    }

    LOG(INFO) << "Exited Listener";
    return (wxThread::ExitCode)0; // everything okay
}

bool ClientListener::isActive() { return this->_isActive; }
