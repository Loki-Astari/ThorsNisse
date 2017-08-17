#include "NisseHandler.h"
#include "Server.h"

#include <iostream>
#include <ctime>

using namespace ThorsAnvil::Nisse::Core::Service;
using TimeVal = struct timeval;

void eventCB(LibSocketId socketId, short eventType, void* event)
{
    NisseHandler& handler = *reinterpret_cast<NisseHandler*>(event);
    handler.activateEventHandlers(socketId, eventType);
}

NisseHandler::NisseHandler(Server& parent, LibSocketId socketId, short eventType, double timeOut)
    : parent(parent)
    , readEvent(nullptr, event_free)
    , writeEvent(nullptr, event_free)
    , suspended(nullptr)
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

struct SetCurrentHandler
{
    std::function<void(NisseHandler*)>  setHandler;
    SetCurrentHandler(std::function<void(NisseHandler*)>&& action, NisseHandler* current)
        : setHandler(std::move(action))
    {
        setHandler(current);
    }
    ~SetCurrentHandler()
    {
        setHandler(nullptr);
    }
};

void NisseHandler::activateEventHandlers(LibSocketId sockId, short eventType)
{
    SetCurrentHandler   setCurrentHandler([&parent=this->parent](NisseHandler* current){parent.setCurrentHandler(current);}, this);
    short newEventType = eventActivate(sockId, eventType);
    setHandlers(newEventType);
}

short NisseHandler::eventActivate(LibSocketId sockId, short eventType)
{
    std::cerr << "Callback made: " << sockId << " For " << eventType << "\n";
    return 0;
}

void NisseHandler::setSuspend(NisseHandler& handlerToSuspend)
{
    suspended = &handlerToSuspend;
    handlerToSuspend.suspend();
}

void NisseHandler::suspend()
{
    // Overridden in base class
    throw std::runtime_error("Not Supported");
}

void NisseHandler::resume()
{
    setHandlers(EV_READ | EV_WRITE);
}

void NisseHandler::dropHandler()
{
    dropEvent();
    parent.delHandler(this);
    if (suspended)
    {
        suspended->resume();
    }
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
#include "ThorsNisseCoreSocket/Socket.h"
template ThorsAnvil::Nisse::Core::Service::ServerHandler<Action, void>::ServerHandler(ThorsAnvil::Nisse::Core::Service::Server&, ThorsAnvil::Nisse::Core::Socket::ServerSocket&&);
template ThorsAnvil::Nisse::Core::Service::ServerHandler<ActionUnReg, void>::ServerHandler(ThorsAnvil::Nisse::Core::Service::Server&, ThorsAnvil::Nisse::Core::Socket::ServerSocket&&);
#endif
