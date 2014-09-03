#include "ClientWebSocketHandler.h"

ClientWebSocketHandler::ClientWebSocketHandler()
{
    m_server.init_asio();
    m_server.set_open_handler(bind(&ClientWebSocketHandler::on_open, this, _1));
    m_server.set_close_handler(bind(&ClientWebSocketHandler::on_close, this, _1));

    m_server.set_access_channels(websocketpp::log::alevel::none);
    m_server.set_error_channels(websocketpp::log::elevel::rerror);
}

ClientWebSocketHandler::~ClientWebSocketHandler()
{
    for (auto it : m_connections)
    {
        m_server.close(it, websocketpp::close::status::normal, "Success");
    }
}

void ClientWebSocketHandler::on_open(connection_hdl hdl)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_connections.insert(hdl);
}

void ClientWebSocketHandler::on_close(connection_hdl hdl)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_connections.erase(hdl);
}

void ClientWebSocketHandler::run(uint16_t port)
{
    m_server.listen(port);
    m_server.start_accept();
    m_server.run();
}

void ClientWebSocketHandler::send(const std::string msg)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto it : m_connections)
    {
        m_server.send(it, msg, websocketpp::frame::opcode::text);
    }
}