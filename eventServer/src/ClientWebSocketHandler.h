#pragma once

#include "_no_copy.h"

#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <set>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

using websocketpp::connection_hdl;

typedef websocketpp::server<websocketpp::config::asio> ws_server;
typedef std::set<connection_hdl, std::owner_less<connection_hdl>> con_list;
typedef websocketpp::server<websocketpp::config::asio>::message_ptr msg_ptr;
typedef websocketpp::server<websocketpp::config::asio>::message_handler msg_hndlr;


class message_handler_class_t
{
public:
    virtual void message_handler(connection_hdl hdl, msg_ptr msg) = 0;
};

class ClientWebSocketHandler : public _no_copy
{
public:

    ClientWebSocketHandler(bool debug = false);
    ~ClientWebSocketHandler();

    virtual void on_open(connection_hdl hdl);
    virtual void on_close(connection_hdl hdl);
    virtual void on_message(connection_hdl hdl, msg_ptr msg);
    virtual void run(uint16_t port);
    virtual void stop();
    virtual void send(const std::string msg);
    virtual void send(connection_hdl hdl, const std::string msg);
    virtual void set_message_handler(message_handler_class_t *cls);
    
private:

    void run_thread();
    ws_server m_server;
    con_list m_connections;
    std::mutex m_mutex;

    message_handler_class_t *handler_class;

    uint16_t port;

    std::unique_ptr<std::thread> thrd;
};