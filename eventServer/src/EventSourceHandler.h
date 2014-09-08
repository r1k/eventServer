#pragma once

#include <cstdint>

#include "_no_copy.h"
#include <set>
#include <string>
#include <memory>
#include <thread>
#include "ClientWebSocketHandler.h"

class appEngine;
class connection_list;

class SourceWebSocketPair_t
{
public:
    std::string             name;  // "multicastip:port"
    uint16_t                ws_port;
    std::shared_ptr<ClientWebSocketHandler> wbskt;
    std::shared_ptr<appEngine>              evntSrc;
    SourceWebSocketPair_t(std::string n, std::shared_ptr<ClientWebSocketHandler> ws, std::shared_ptr<appEngine> ae) :
        name(n),
        wbskt(ws),
        evntSrc(ae)
    { }
};
inline bool operator<(const SourceWebSocketPair_t& lhs, const SourceWebSocketPair_t& rhs)
{ 
    return lhs.name < rhs.name;
}
inline bool operator<(const std::shared_ptr<SourceWebSocketPair_t> &lhs, const std::shared_ptr<SourceWebSocketPair_t>& rhs)
{
    return *lhs < *rhs;
}
typedef std::set<SourceWebSocketPair_t> event_socket_list_t;


class EventSourceHandler : public message_handler_class_t, public _no_copy
{
public:
    EventSourceHandler(uint16_t port) : base_port(port), next_port(port + 1) { CreateControlPort(); }
    ~EventSourceHandler() {}

    void CreateControlPort();

    void CreateStreamServer(std::string multicast_ip,
                            uint16_t multicast_port,
                            std::string multicast_if);

    void DeleteStreamServer(std::string multicast_ip, uint16_t multicast_port);
    void StopStreamServer(const SourceWebSocketPair_t& pair);
    void stop_all();

    virtual void message_handler(connection_hdl hdl, msg_ptr p_msg);

private:

    void BroadcastServerListEvent();
    void SendServerListEvent(connection_hdl hdl);

    std::shared_ptr<connection_list> GetServerConnectionList();

    std::shared_ptr<ClientWebSocketHandler> controlPort;
    event_socket_list_t connection_pair_list;
    const uint16_t base_port;
    uint16_t next_port;
};