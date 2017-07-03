#include "NisseHandler.h"

#include <iostream>

using namespace ThorsAnvil::Nisse;

void eventCB(LibSocketId socketId, short eventType, void* event)
{
    NisseEvent* eventObj = reinterpret_cast<NisseEvent*>(event);
    eventObj->eventActivate(socketId, eventType);
}

NisseEvent::NisseEvent(LibEventBase* base, LibSocketId socketId)
    : event(event_new(base, socketId,  EV_READ | EV_PERSIST, eventCB, this))
{
    if (event == nullptr)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::NisseEvent::NisseEvent: event_new(): Failed");
    }
    if (event_add(event, nullptr) != 0)
    {
        event_free(event);
        throw std::runtime_error("ThorsAnvil::Nisse::NisseEvent::NisseEvent: event_add(): Failed");
    }
}

NisseEvent::~NisseEvent()
{
    if (event_del(event) != 0)
    {
        //throw std::runtime_error("ThorsAnvil::Nisse::NisseEvent::NisseEvent: event_del(): Failed");
    }
    event_free(event);
}

void NisseEvent::eventActivate(LibSocketId sockId, short eventType)
{
    std::cout << "Callback made: " << sockId << " For " << eventType << "\n";
}

ServerEvent::ServerEvent(LibEventBase* base, int port)
    : socket(port)
    , eventListener(base, socket.getSocketId())
{}
