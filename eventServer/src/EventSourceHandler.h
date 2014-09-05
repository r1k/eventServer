#pragma once

#include <cstdint>

#include "_no_copy.h"
#include <set>
#include <string>
#include <memory>

class ClientWebSocketHandler;
class appEngine;

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


class EventSourceHandler : public _no_copy
{
public:
    EventSourceHandler(uint16_t port) : base_port(port), next_port(port) {}
    ~EventSourceHandler() {}

    void CreateStreamServer(std::string multicast_ip,
                            uint16_t multicast_port,
                            std::string multicast_if);

    void DeleteStreamServer(std::string multicast_ip, uint16_t multicast_port);
    void StopStreamServer(const SourceWebSocketPair_t& pair);
    void stop_all();
private:

    void SendServerListEvent();
    event_socket_list_t connection_pair_list;
    const uint16_t base_port;
    uint16_t next_port;
};