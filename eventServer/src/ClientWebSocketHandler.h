#pragma once

#include "_no_copy.h"

#include <functional>
#include <memory>
#include <mutex>
#include <thread>
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

    virtual void on_open(connection_hdl hdl);
    virtual void on_close(connection_hdl hdl);
    virtual void run(uint16_t port);
    virtual void stop();
    virtual void send(const std::string msg);

private:

    void run_thread();
    ws_server m_server;
    con_list m_connections;
    std::mutex m_mutex;

    uint16_t port;

    std::unique_ptr<std::thread> thrd;
};