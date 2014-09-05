#pragma once

#include <thread>
#include "mpeg/inputs/CTransportStreamMulticast.h"
#include "mpeg/inputs/CTransportStreamFile.h"
#include "mpeg/engines/CTransportStreamPreProcessor.h"
#include "mpeg/data/CTransportStreamPacketBuffer.h"
#include "mpeg/events/CTransportStreamEvent.h"
#include "mpeg/events/CBitrateListEvent.h"
#include "mpeg/events/CBitrateEvent.h"
#include "mpeg/events/CPidEvent.h"
#include "net/CIpAddress.h"
#include "event/IEventRegister.h"
#include "event/CAbsEventHandler.h"
#include "event/CEventDispatcher.h"
#include "console/CFileTraceListener.h"

#include "ClientWebSocketHandler.h"

using namespace std;
using namespace themelios;


class appEngine : public CAbsEventHandler<CTransportStreamEvent>
{
private:
    // multicast details
    const string   source_ipMulticast;
    const string   source_ipInterface;
    unsigned short source_port;

    // transport stream source
    ITransportStreamSource*         source;
    CTransportStreamPreProcessor*   preProc;

    ClientWebSocketHandler &websocket;

    CEventDispatcher eventDispatcher;

    std::unique_ptr<std::thread> thrd;

public:
    appEngine(string Multicast_IP,
        uint16_t Multicast_Port,
        string Multicast_IF,
        ClientWebSocketHandler &wsh)
        : source_ipMulticast(Multicast_IP),
        source_ipInterface(Multicast_IF),
        source_port(Multicast_Port),
        source(nullptr),
        preProc(nullptr),
        websocket(wsh)
    {
        createSource();
    }

    appEngine(string Multicast_IP,
        uint16_t Multicast_Port,
        ClientWebSocketHandler &wsh)
        : source_ipMulticast(Multicast_IP),
        source_ipInterface("0.0.0.0"),
        source_port(Multicast_Port),
        source(nullptr),
        preProc(nullptr),
        websocket(wsh)
    {
        createSource();
    }

    ~appEngine()
    {
        if (source)  delete source;
        if (preProc) delete preProc;
    }

    void createSource();

    virtual bool eventFilter(const CTransportStreamEvent & ev);

    virtual void onEvent(const CTransportStreamEvent & ev);

    void packetProcessor();

    void run();
    void stop();
};
