#include "EventEngine.h"
#include "make_unique.h"

#include "mpeg/inputs/CTransportStreamMulticast.h"
#include "mpeg/inputs/CTransportStreamFile.h"
#include "mpeg/data/CTransportStreamPacketBuffer.h"
#include "mpeg/events/CBitrateListEvent.h"
#include "mpeg/events/CBitrateEvent.h"
#include "mpeg/events/CPidEvent.h"
#include "net/CIpAddress.h"
#include "event/IEventRegister.h"
#include "console/CFileTraceListener.h"

void appEngine::createSource()
{
    CTransportStreamMulticast *pMulticast = new CTransportStreamMulticast(CIpAddress(source_ipMulticast), source_port, CIpAddress(source_ipInterface));
    pMulticast->setRetryConnection(true);
    source = pMulticast;

    // create the preprocessor
    preProc = new CTransportStreamPreProcessor(eventDispatcher);

    // also register the "config" structure to the dispatcher
    registerHandler(eventDispatcher);
}

bool appEngine::eventFilter(const CTransportStreamEvent & ev)
{
    bool wanted = false;
    const CTransportStreamEvent *pEv = &ev;

    const CPidEvent * pe = NULL;
    const CBitrateEvent * bre = NULL;
    const CBitrateListEvent * brle = NULL;

    if (pe = dynamic_cast<const CPidEvent *>(pEv))
    {
        if (pe->isDisappearance())
            wanted = false;
    }
    else if (bre = dynamic_cast<const CBitrateEvent *>(pEv))
    {
        // cout << "Bitrate message for PID:" << bre->Pid << endl;
        wanted = false;
    }
    else if (brle = dynamic_cast<const CBitrateListEvent *>(pEv))
    {
        // cout << "Sending bitrate list message" << endl;
        wanted = true;
    }

    return wanted;
}

void appEngine::onEvent(const CTransportStreamEvent & ev)
{
    if (eventFilter(ev))
    {
        const string json = ev.toJSON();
        websocket.send(json);
    }
}

void appEngine::packetProcessor()
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

void appEngine::run()
{
    thrd = make_unique<thread>(&appEngine::packetProcessor, this);
}

void appEngine::stop()
{

}
