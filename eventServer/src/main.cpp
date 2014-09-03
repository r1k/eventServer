#include <iostream>
#include <string>
#include <sstream>

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

#include"_no_copy.h"
#include "CommandLineParse.h"
#include "ClientWebSocketHandler.h"

#include <list>
#include <functional>
#include <memory>
#include <thread>

#include <boost/asio.hpp>
#include "boost_http/server/server.hpp"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
namespace fs = boost::filesystem;

using namespace std;
using namespace themelios;

//themelios::CFileTraceListener fileListener("console.log");

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

public:
    appEngine(string Multicast_IP, 
              int Multicast_Port, 
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
              int Multicast_Port,
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

    void createSource()
    {
        CTransportStreamMulticast *pMulticast = new CTransportStreamMulticast(CIpAddress(source_ipMulticast), source_port, CIpAddress(source_ipInterface));
        pMulticast->setRetryConnection(true);
        source = pMulticast;

        // create the preprocessor
        preProc = new CTransportStreamPreProcessor(eventDispatcher);

        // also register the "config" structure to the dispatcher
        registerHandler(eventDispatcher);
    }
    
    virtual bool eventFilter(const CTransportStreamEvent & ev)
    {
        bool wanted = false;
        const CTransportStreamEvent *pEv = &ev;

        const CPidEvent * pe = NULL;
        const CBitrateEvent * bre = NULL;
        const CBitrateListEvent * brle = NULL;

        if (
            (pe = dynamic_cast<const CPidEvent *>(pEv))
            ||
            (bre = dynamic_cast<const CBitrateEvent *>(pEv))
            ||
            (brle = dynamic_cast<const CBitrateListEvent *>(pEv))
            )
        {
            if (pe)
            {
                if(pe->isDisappearance())
                     wanted = false;
            }
            else if (bre)
            {
                // cout << "Bitrate message for PID:" << bre->Pid << endl;
                wanted = false;
            }
            else if (brle)
            {
                // cout << "Sending bitrate list message" << endl;
                wanted = true;
            }
        }

        return wanted;
    }

    virtual void onEvent(const CTransportStreamEvent & ev)
    {
        if (eventFilter(ev))
        {
            const string json = ev.toJSON();
            websocket.send(json);
        }
    }

    void packetProcessor()
    {
        for (;;)
        {
            try
            {
                // get the packet
                const CTransportStreamPacketBuffer& pkt = source->getNextPacket();

                // push to the preproc
                preProc->pushNextPacket(pkt);
            }
            catch (underflow_error& ue)
            {
                cout << "Error:" << __FILE__ << ":" << __LINE__ << ":Exception from packet handling detected - " << ue.what() << endl;
                // let's try to carry on from this
            }
        }
    }
};

class HTTPServer : public _no_copy
{
private:
    unique_ptr<http::server::server> server;
public:
    HTTPServer(const string listening_interface, const int port, const string doc_root)
    {
        try
        {
            stringstream port_s;
            port_s << port;
            const string port_string(port_s.str());
            server = make_unique<http::server::server>(listening_interface, port_string, doc_root);
            server->run();
        }
        catch (std::exception& e)
        {
            std::cerr << "exception: " << e.what() << "\n";
        }
    }

    ~HTTPServer()
    {
        
    }
};


string getDocRoot(char *argv0)
{
    fs::path full_path(fs::initial_path<fs::path>());

    full_path = fs::system_complete(fs::path(argv0));

    return full_path.parent_path().string() + "/doc_root/";
}

int main(int argc, char** argv)
{
    CommandLineParse clp(argc, argv);

    string       multicastIPAddress("0.0.0.0");
    int          multicastPortNumber = 5000;
    string       multicastInterface("0.0.0.0");
    bool         interfaceSet = false;
    int          basePort = 8080;
    bool         okay = true;
    const string doc_root = getDocRoot(argv[0]);

    okay = clp.GetStringValue(1, multicastIPAddress);
    okay = clp.GetNumValue(2, multicastPortNumber);
    okay = clp.GetNumValue(3, basePort);

    const int httpReqPort = basePort;
    const int webSocketPort = basePort + 1;
    
    if (!okay)
    {
        cerr << "Incorrect usage:" << endl;
        cerr << " eventServer <multicast IP Address> <multicast port> <base port> [options]" << endl;
        cerr << "     available options:" << endl;
        cerr << "         -i <interface address>" << endl << endl;
        cerr << " This program will open a port on base port and a further port on base port + 1" << endl;

        return -1;
    }

    interfaceSet = clp.GetStringValue("-i", multicastInterface);

    bool deadInTheWater = false;
    while (!deadInTheWater)
    {
        //HTTPServer httpServer("0.0.0.0", httpReqPort, doc_root);
        ClientWebSocketHandler webSockHander;
        appEngine server(multicastIPAddress, 
                         multicastPortNumber, 
                         multicastInterface, 
                         webSockHander);
        try
        {           
            std::thread t(std::bind(&appEngine::packetProcessor, &server));
            webSockHander.run(webSocketPort);
        }
        catch (exception& ex)
        {
            cout << "Error:" << __FILE__ << ":" << __LINE__ << ":Exception detected - " << ex.what() << endl;
            return -1;
        }
    }
    return 0;
}
