#include "NisseHandler.h"
#include "NisseService.h"

#include <iostream>

using namespace ThorsAnvil::Nisse;

void eventCB(LibSocketId socketId, short eventType, void* event)
{
    NisseHandler& handler = *reinterpret_cast<NisseHandler*>(event);
    handler.eventActivate(socketId, eventType);
}

NisseHandler::NisseHandler(NisseService& parent, LibEventBase* base, LibSocketId socketId, short eventType)
    : parent(parent)
    , event(event_new(base, socketId, eventType | EV_PERSIST, eventCB, this), event_free)
{
    if (event.get() == nullptr)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::NisseHandler::NisseHandler: event_new(): Failed");
    }
    if (event_add(event.get(), nullptr) != 0)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::NisseHandler::NisseHandler: event_add(): Failed");
    }
}

void NisseHandler::eventActivate(LibSocketId sockId, short eventType)
{
    std::cerr << "Callback made: " << sockId << " For " << eventType << "\n";
}

void NisseHandler::dropHandler()
{
    if (event_del(event.get()) != 0)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::NisseEvent::dropHandler: event_del(): Failed");
    }
    parent.delHandler(this);
}

#ifdef COVERAGE_TEST
/*
 * This code is only compiled into the unit tests for code coverage purposes
 * It is not part of the live code.
 */
#include "NisseHandler.tpp"
#include "test/Action.h"
#include "ThorsNisseSocket/Socket.h"
template ThorsAnvil::Nisse::ServerHandler<Action, void>::ServerHandler(ThorsAnvil::Nisse::NisseService&, event_base*, ThorsAnvil::Socket::ServerSocket&&);
template ThorsAnvil::Nisse::ServerHandler<ActionUnReg, void>::ServerHandler(ThorsAnvil::Nisse::NisseService&, event_base*, ThorsAnvil::Socket::ServerSocket&&);
#endif
