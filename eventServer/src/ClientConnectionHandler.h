#pragma once

#include <websocketpp/server.hpp>

using websocketpp::connection_hdl;
class ClientConnectionHandler;
typedef std::weak_ptr<ClientConnectionHandler> wClientConnectionHandler;

class ClientConnectionHandler
{
public:
    ClientConnectionHandler(connection_hdl hdle)
    {
        hdl = hdle;
    }

    ~ClientConnectionHandler()
    {

    }

    connection_hdl handle() const
    {
        return hdl;
    }

private:

    connection_hdl hdl;
};
