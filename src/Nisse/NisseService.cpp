#include "NisseService.h"

#include <iostream>
#pragma vera_pushoff
using TimeVal = struct timeval;
#pragma vera_pop

using namespace ThorsAnvil::Nisse;
using TimeVal = struct timeval;

NisseService::NisseService()
    : running(true)
    , eventBase(event_base_new())
{
    if (eventBase == nullptr)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::NisseService::NisseService: event_base_new(): Failed");
    }
}

NisseService::~NisseService()
{
}

void NisseService::start()
{
    std::cout << "Nisse Started\n";
    while (running)
    {
        std::cout << "Nisse Loop\n";
        runLoop();
    }
    std::cout << "Nisse Stopped\n";
}

void NisseService::flagShutDown()
{
    running = false;
    if (event_base_loopbreak(eventBase) != 0)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::NisseService::flagShutDown: event_base_loopbreak(): Failed");
    }
}

void NisseService::runLoop()
{
    static const TimeVal ten_sec{10,0};

    if (event_base_loopexit(eventBase, &ten_sec) != 0)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::NisseService::runLoop: event_base_loopexit(): Failed");
    }
    switch (event_base_dispatch(eventBase))
    {
        case -1:
            throw std::runtime_error("ThorsAnvil::Nisse::NisseService::runLoop: event_base_dispatch(): Failed");
        case 1:
            throw std::runtime_error("ThorsAnvil::Nisse::NisseService::runLoop: event_base_dispatch(): No Events");
        case 0:
            break;
        default:
            throw std::runtime_error("ThorsAnvil::Nisse::NisseService::runLoop: event_base_dispatch(): Unknown Error");
    }
}

void NisseService::listenOn(int port)
{
    new ServerEvent(eventBase, port);
}

// export "C"
void eventCB(LibSocketId socketId, short eventType, void* event)
{
    NisseEvent* eventObj = reinterpret_cast<NisseEvent*>(event);
    eventObj->eventActivate(socketId, eventType);
}

void NisseEvent::eventActivate(LibSocketId sockId, short eventType)
{
    std::cout << "Callback made: " << sockId << " For " << eventType << "\n";
}
