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

    ClientWebSocketHandler()
    {
        m_server.init_asio();
        m_server.set_open_handler(bind(&ClientWebSocketHandler::on_open, this, _1));
        m_server.set_close_handler(bind(&ClientWebSocketHandler::on_close, this, _1));
    }

    ~ClientWebSocketHandler()
    {
        for (auto it : m_connections)
        {
            m_server.close(it, websocketpp::close::status::normal, "Success");
        }
    }

    void on_open(connection_hdl hdl)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_connections.insert(hdl);
    }

    void on_close(connection_hdl hdl)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_connections.erase(hdl);
    }

    void run(uint16_t port)
    {
        m_server.listen(port);
        m_server.start_accept();
        m_server.run();
    }

    virtual void send(const std::string msg)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (auto it : m_connections)
        {
            m_server.send(it, msg, websocketpp::frame::opcode::text);
        }
    }
private:
    ws_server m_server;
    con_list m_connections;
    std::mutex m_mutex;
};