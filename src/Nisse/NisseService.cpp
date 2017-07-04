#include "NisseService.h"
#include "NisseHandler.h"

#include <iostream>
using namespace ThorsAnvil::Nisse;

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
    addHandler<ServerHandler>(ThorsAnvil::Socket::ServerSocket(port));
}
