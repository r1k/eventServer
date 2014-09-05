#include "EventSourceHandler.h"

#include "ClientWebSocketHandler.h"
#include "EventEngine.h"
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;

void EventSourceHandler::CreateStreamServer(std::string multicast_ip,
    uint16_t multicast_port,
    std::string multicast_if)
{
    shared_ptr<ClientWebSocketHandler> webSockHandler = make_shared<ClientWebSocketHandler>();
    shared_ptr<appEngine> ae = make_shared<appEngine>(multicast_ip, multicast_port, multicast_if, *webSockHandler);
    stringstream name;
    name << multicast_ip << ":" << multicast_port;

    SourceWebSocketPair_t pair(name.str(), webSockHandler, ae);

    connection_pair_list.insert(pair);

    // now start all going

    ae->run();
    webSockHandler->run(next_port++);
}

void EventSourceHandler::DeleteStreamServer(std::string multicast_ip, uint16_t multicast_port)
{
    stringstream key;
    key << multicast_ip << ":" << multicast_port;

    for (auto item : connection_pair_list)
    {
        shared_ptr<SourceWebSocketPair_t> local_p;
        if (key.str() == item.name)
        {
            StopStreamServer(item);
            connection_pair_list.erase(item);
        }
    }
}

void EventSourceHandler::StopStreamServer(const SourceWebSocketPair_t& pair)
{
    pair.evntSrc->stop();
    pair.wbskt->stop();
}

struct connection
{
    string src;
    string port;
};

class connection_list
{
public:
    list<connection> data;
    string to_json_string() 
    {
        boost::property_tree::ptree root_pt, servers;
        root_pt.put("type", "stream_list");
        for (auto item : data)
        {
            boost::property_tree::ptree pt;
            pt.put("source", item.src);
            pt.put("port", item.port);

            servers.push_back(std::make_pair("", pt));
        }
        root_pt.add_child("streams", servers);

        std::stringstream ss;
        boost::property_tree::json_parser::write_json(ss, root_pt);
        return ss.str();
    }
};

void EventSourceHandler::SendServerListEvent()
{
    // Send a list of servers to all clients

    connection_list connections;
    for (auto ws : connection_pair_list)
    {
        connection c;
        c.src = ws.name;
        stringstream port;
        port << ws.ws_port;
        c.port = port.str();
        // convert connection details to json string and store
        connections.data.push_back(c);
    }

    string msg = connections.to_json_string();

    for (auto ws : connection_pair_list)
    {
        // send message to all clients
        ws.wbskt->send(msg);
    }
}

void EventSourceHandler::stop_all()
{
    for (auto ws : connection_pair_list)
    {
        StopStreamServer(ws);
    }
}
