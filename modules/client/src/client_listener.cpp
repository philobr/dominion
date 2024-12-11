#include "client_listener.h"
#include <shared/utils/logger.h>
#include "client_network_manager.h"


ClientListener::ClientListener(sockpp::tcp_connector *connection) :
    wxThread(wxTHREAD_DETACHED), _connection(connection), _isActive(true), _listenerExited(false)
{}


ClientListener::~ClientListener()
{
    // Deconstructor is called from a differnt thread, so we have to let the loop know to terminate
    this->_isActive = false;
    // Wait for listener loop to exit
    while ( !listenerExited() ) {
    };
}


wxThread::ExitCode ClientListener::Entry()
{
    try {
        char buffer[512]; // 512 bytes
        ssize_t count = 0;

        this->_connection->set_non_blocking();

        while ( this->isActive() &&
                ((count = this->_connection->read(buffer, sizeof(buffer))) > 0 ||
                 this->_connection->last_error() == EWOULDBLOCK) ) {
            try {
                int pos = 0;

                // extract length of message in bytes (which is sent at the start of the message, and is separated by a
                // ":")
                std::stringstream messageLengthStream;
                while ( pos < count && buffer[pos] != ':' ) {
                    messageLengthStream << buffer[pos];
                    pos++;
                }
                ssize_t messageLength = std::stoi(messageLengthStream.str());

                // initialize a stream for the message
                std::stringstream messageStream;

                // copy everything following the message length declaration into a stringstream
                messageStream.write(&buffer[pos + 1], count - (pos + 1));
                ssize_t bytesReadSoFar = count - (pos + 1);

                // read remaining packages until full message length is reached
                while ( bytesReadSoFar < messageLength && count != 0 ) {
                    count = this->_connection->read(buffer, sizeof(buffer));
                    messageStream.write(buffer, count);
                    bytesReadSoFar += count;
                }

                // process message (if we've received entire message)
                if ( bytesReadSoFar == messageLength ) {
                    std::string message = messageStream.str();
                    // GameController::getMainThreadEventHandler()->CallAfter([message]{
                    ClientNetworkManager::receiveMessage(message);
                    //});

                } else {
                    LOG(ERROR) << "Network error. Could not read entire message. TCP stream ended early. Difference is "
                               << std::to_string(messageLength - bytesReadSoFar) << " bytes";
                }

            } catch ( std::exception &e ) {
                // Make sure the connection isn't terminated only because of a read error
                LOG(ERROR) << "Network error. Error while reading message: " << std::string(e.what());
            }
        }

        LOG(ERROR) << "Network error. Read error, shutting down Listener";

    } catch ( const std::exception &e ) {
        LOG(ERROR) << "Network error. Error in listener thread: " << std::string(e.what());
    }

    LOG(INFO) << "Exited Listener";
    _listenerExited = true;
    return (wxThread::ExitCode)0; // everything okay
}

bool ClientListener::isActive() { return this->_isActive; }
bool ClientListener::listenerExited() { return this->_listenerExited; }
