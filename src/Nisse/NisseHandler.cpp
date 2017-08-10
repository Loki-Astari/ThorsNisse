#include "NisseHandler.h"
#include "NisseService.h"

#include <iostream>
#include <ctime>

using namespace ThorsAnvil::Nisse;
using TimeVal = struct timeval;

void eventCB(LibSocketId socketId, short eventType, void* event)
{
    NisseHandler& handler = *reinterpret_cast<NisseHandler*>(event);
    short newEventType = handler.eventActivate(socketId, eventType);
    handler.setHandlers(newEventType);
}

NisseHandler::NisseHandler(NisseService& parent, LibSocketId socketId, short eventType, double timeOut)
    : parent(parent)
    , readEvent(nullptr, event_free)
    , writeEvent(nullptr, event_free)
{
    short persistType = eventType & EV_PERSIST;
    short readType    = eventType & EV_READ;

    readEvent.reset(event_new(parent.eventBase.get(), socketId, readType | persistType, eventCB, this));
    if (readEvent.get() == nullptr)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::NisseHandler::NisseHandler: readEvent: event_new(): Failed");
    }
    writeEvent.reset(event_new(parent.eventBase.get(), socketId, EV_WRITE | persistType, eventCB, this));
    if (writeEvent.get() == nullptr)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::NisseHandler::NisseHandler: writeEvent: event_new(): Failed");
    }

    TimeVal* timeVal = nullptr;
    TimeVal  timer;
    if (timeOut != 0)
    {
        timer.tv_sec    = static_cast<std::time_t>(timeOut);
        timer.tv_usec   = static_cast<long>((timeOut - timer.tv_sec) * 1'000'000);
        timeVal         = &timer;
    }
    setHandlers(eventType, timeVal);
}


NisseHandler::~NisseHandler()
{
    dropEvent();
}

short NisseHandler::eventActivate(LibSocketId sockId, short eventType)
{
    std::cerr << "Callback made: " << sockId << " For " << eventType << "\n";
    return 0;
}

void NisseHandler::dropHandler()
{
    dropEvent();
    parent.delHandler(this);
}

void NisseHandler::dropEvent()
{
    if (event_del(readEvent.get()) != 0)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::NisseEvent::dropHandler: readEvent event_del(): Failed");
    }
    if (event_del(writeEvent.get()) != 0)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::NisseEvent::dropHandler: writeEvent event_del(): Failed");
    }
}

void NisseHandler::setHandlers(short eventType, TimeVal* timeVal)
{
    if (timeVal != nullptr || (eventType & EV_READ))
    {
        if (event_add(readEvent.get(), timeVal) != 0)
        {
            throw std::runtime_error("ThorsAnvil::Nisse::NisseHandler::NisseHandler: readEvent: event_add(): Failed");
        }
    }
    if (eventType & EV_WRITE)
    {
        if (event_add(writeEvent.get(), timeVal) != 0)
        {
            throw std::runtime_error("ThorsAnvil::Nisse::NisseHandler::NisseHandler: writeEvent: event_add(): Failed");
        }
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
