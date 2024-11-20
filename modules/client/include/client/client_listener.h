#pragma once

#include <functional>
#include <wx/wx.h>
#include "sockpp/tcp_connector.h"
#include "sockpp/tcp_socket.h"


class ClientListener : public wxThread
{

public:
    ClientListener(sockpp::tcp_connector *connection);
    ~ClientListener();

protected:
    virtual ExitCode Entry();

private:
    void outputError(const std::string &title, const std::string &message);


    sockpp::tcp_connector *_connection;
};
