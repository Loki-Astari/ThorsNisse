#include "NisseHandler.h"
#include "NisseService.h"

#include <iostream>

using namespace ThorsAnvil::Nisse;

void eventCB(LibSocketId socketId, short eventType, void* event)
{
    NisseHandler& handler = *reinterpret_cast<NisseHandler*>(event);
    handler.eventActivate(socketId, eventType);
}

NisseEvent::NisseEvent(LibEventBase* base, LibSocketId socketId, NisseHandler& parent, short eventType)
    : event(event_new(base, socketId,  eventType | EV_PERSIST, eventCB, &parent))
{
    if (event == nullptr)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::NisseEvent::NisseEvent: event_new(): Failed");
    }
    if (event_add(event, nullptr) != 0)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::NisseEvent::NisseEvent: event_add(): Failed");
    }
}

NisseEvent::~NisseEvent()
{
    event_free(event);
}

void NisseEvent::drop()
{
    if (event_del(event) != 0)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::NisseEvent::drop: event_del(): Failed");
    }
}

NisseHandler::NisseHandler(NisseService& parent, LibEventBase* base, LibSocketId socketId)
    : parent(parent)
    , eventListener(base, socketId, *this, EV_READ)
{}

void NisseHandler::eventActivate(LibSocketId sockId, short eventType)
{
    std::cerr << "Callback made: " << sockId << " For " << eventType << "\n";
}

#ifdef COVERAGE_TEST
/*
 * This code is only compiled into the unit tests for code coverage purposes
 * It is not part of the live code.
 */
#include "NisseHandler.tpp"
#endif
