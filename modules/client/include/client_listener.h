#pragma once

#include <wx/wx.h>
#include "sockpp/tcp_connector.h"


class ClientListener : public wxThread
{

public:
    ClientListener(sockpp::tcp_connector *connection);
    ~ClientListener() override;

protected:
    ExitCode Entry() override;

private:
    void outputError(const std::string &title, const std::string &message);
    bool isActive();
    bool listenerExited();


    sockpp::tcp_connector *_connection;

    bool _isActive;
    bool _listenerExited;
};
