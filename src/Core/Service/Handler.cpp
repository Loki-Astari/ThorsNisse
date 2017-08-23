#include "Handler.h"
#include "Server.h"

#include <iostream>
#include <ctime>

using namespace ThorsAnvil::Nisse::Core::Service;
using TimeVal = struct timeval;

void eventCB(LibSocketId socketId, short eventType, void* event)
{
    Handler& handler = *reinterpret_cast<Handler*>(event);
    handler.activateEventHandlers(socketId, eventType);
}

Handler::Handler(Server& parent, LibSocketId socketId, short eventType, double timeOut)
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
        throw std::runtime_error("ThorsAnvil::Nisse::Handler::Handler: readEvent: event_new(): Failed");
    }
    writeEvent.reset(event_new(parent.eventBase.get(), socketId, EV_WRITE | persistType, eventCB, this));
    if (writeEvent.get() == nullptr)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::Handler::Handler: writeEvent: event_new(): Failed");
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


Handler::~Handler()
{
    dropEvent();
}

struct SetCurrentHandler
{
    std::function<void(Handler*)>  setHandler;
    SetCurrentHandler(std::function<void(Handler*)>&& action, Handler* current)
        : setHandler(std::move(action))
    {
        setHandler(current);
    }
    ~SetCurrentHandler()
    {
        setHandler(nullptr);
    }
};

void Handler::activateEventHandlers(LibSocketId sockId, short eventType)
{
    SetCurrentHandler   setCurrentHandler([&parent=this->parent](Handler* current){parent.setCurrentHandler(current);}, this);
    short newEventType = eventActivate(sockId, eventType);
    setHandlers(newEventType);
}

short Handler::eventActivate(LibSocketId sockId, short eventType)
{
    std::cerr << "Callback made: " << sockId << " For " << eventType << "\n";
    return 0;
}

void Handler::setSuspend(Handler& handlerToSuspend)
{
    suspended = &handlerToSuspend;
    handlerToSuspend.suspend();
}

void Handler::suspend()
{
    // Overridden in base class
    throw std::runtime_error("ThorsAnvil::Nisse::Handler::suspend: Failed");
}

void Handler::resume()
{
    setHandlers(EV_READ | EV_WRITE);
}

void Handler::dropHandler()
{
    dropEvent();
    parent.delHandler(this);
    if (suspended)
    {
        suspended->resume();
    }
}

void Handler::dropEvent()
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

void Handler::setHandlers(short eventType, TimeVal* timeVal)
{
    if (timeVal != nullptr || (eventType & EV_READ))
    {
        if (event_add(readEvent.get(), timeVal) != 0)
        {
            throw std::runtime_error("ThorsAnvil::Nisse::Handler::Handler: readEvent: event_add(): Failed");
        }
    }
    if (eventType & EV_WRITE)
    {
        if (event_add(writeEvent.get(), timeVal) != 0)
        {
            throw std::runtime_error("ThorsAnvil::Nisse::Handler::Handler: writeEvent: event_add(): Failed");
        }
    }
}

#ifdef COVERAGE_TEST
/*
 * This code is only compiled into the unit tests for code coverage purposes
 * It is not part of the live code.
 */
#include "Server.tpp"
#include "Handler.tpp"
#include "test/Action.h"
#include "ThorsNisseCoreSocket/Socket.h"
template ThorsAnvil::Nisse::Core::Service::ServerHandler<Action, void>::ServerHandler(ThorsAnvil::Nisse::Core::Service::Server&, ThorsAnvil::Nisse::Core::Socket::ServerSocket&&);
template ThorsAnvil::Nisse::Core::Service::ServerHandler<ActionUnReg, void>::ServerHandler(ThorsAnvil::Nisse::Core::Service::Server&, ThorsAnvil::Nisse::Core::Socket::ServerSocket&&);
template void ThorsAnvil::Nisse::Core::Service::Server::listenOn<TestHandler, std::__1::tuple<bool, bool, bool> >(int, std::__1::tuple<bool, bool, bool>&);
#endif
