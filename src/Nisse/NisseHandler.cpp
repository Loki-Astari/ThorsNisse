#include "NisseHandler.h"
#include "NisseService.h"

#include <iostream>

using namespace ThorsAnvil::Nisse;
using TimeVal = struct timeval;

void eventCB(LibSocketId socketId, short eventType, void* event)
{
    NisseHandler& handler = *reinterpret_cast<NisseHandler*>(event);
    handler.eventActivate(socketId, eventType);
}

NisseHandler::NisseHandler(NisseService& parent, LibSocketId socketId, short eventType, double timeOut)
    : parent(parent)
    , event(event_new(parent.eventBase.get(), socketId, eventType | EV_PERSIST, eventCB, this), event_free)
{
    if (event.get() == nullptr)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::NisseHandler::NisseHandler: event_new(): Failed");
    }

    TimeVal* timeVal = nullptr;
    TimeVal  timer;
    if (timeOut != 0)
    {
        timer.tv_sec    = static_cast<std::time_t>(timeOut);
        timer.tv_usec   = static_cast<long>((timeOut - timer.tv_sec) * 1'000'000);
        timeVal         = &timer;
    }

    if (event_add(event.get(), timeVal) != 0)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::NisseHandler::NisseHandler: event_add(): Failed");
    }
}

NisseHandler::~NisseHandler()
{
    dropEvent();
}

void NisseHandler::eventActivate(LibSocketId sockId, short eventType)
{
    std::cerr << "Callback made: " << sockId << " For " << eventType << "\n";
}

void NisseHandler::dropHandler()
{
    dropEvent();
    parent.delHandler(this);
}

void NisseHandler::dropEvent()
{
    if (event_del(event.get()) != 0)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::NisseEvent::dropHandler: event_del(): Failed");
    }
}

#ifdef COVERAGE_TEST
/*
 * This code is only compiled into the unit tests for code coverage purposes
 * It is not part of the live code.
 */
#include "NisseHandler.tpp"
#include "test/Action.h"
#include "ThorsNisseSocket/Socket.h"
template ThorsAnvil::Nisse::ServerHandler<Action, void>::ServerHandler(ThorsAnvil::Nisse::NisseService&, ThorsAnvil::Socket::ServerSocket&&);
template ThorsAnvil::Nisse::ServerHandler<ActionUnReg, void>::ServerHandler(ThorsAnvil::Nisse::NisseService&, ThorsAnvil::Socket::ServerSocket&&);
#endif
