#pragma once

#include "_no_copy.h"

#include <functional>
#include <memory>
#include <mutex>
#include <set>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

using websocketpp::connection_hdl;

typedef websocketpp::server<websocketpp::config::asio> ws_server;
typedef std::set<connection_hdl, std::owner_less<connection_hdl>> con_list;

class ClientWebSocketHandler : public _no_copy
{
public:

    ClientWebSocketHandler();
    ~ClientWebSocketHandler();

    void on_open(connection_hdl hdl);
    void on_close(connection_hdl hdl);
    void run(uint16_t port);

    virtual void send(const std::string msg);

private:
    ws_server m_server;
    con_list m_connections;
    std::mutex m_mutex;
};