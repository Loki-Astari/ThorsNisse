#include "Handler.h"
#include "Server.h"

#include <iostream>
#include <ctime>

using namespace ThorsAnvil::Nisse::Core::Service;
using TimeVal = struct timeval;

void eventCB(LibSocketId socketId, short eventType, void* event)
{
    HandlerBase* handler = reinterpret_cast<HandlerBase*>(event);
    handler->activateEventHandlers(socketId, eventType);
}

HandlerBase::HandlerBase(Server& parent, LibSocketId socketId, short eventType, double timeOut)
    : parent(parent)
    , readEvent(nullptr, ::event_free)
    , writeEvent(nullptr,::event_free)
    , suspended(nullptr)
{
    short persistType = eventType & EV_PERSIST;
    short readType    = eventType & EV_READ;

    readEvent.reset(::event_new(parent.eventBase.get(), socketId, readType | persistType, eventCB, this));
    if (readEvent.get() == nullptr)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::HandlerBase::Handler: readEvent: event_new(): Failed");
    }
    writeEvent.reset(::event_new(parent.eventBase.get(), socketId, EV_WRITE | persistType, eventCB, this));
    if (writeEvent.get() == nullptr)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::HandlerBase::Handler: writeEvent: event_new(): Failed");
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


HandlerBase::~HandlerBase()
{
    dropEvent();
}

struct SetCurrentHandler
{
    std::function<void(HandlerBase*)>  setHandler;
    SetCurrentHandler(std::function<void(HandlerBase*)>&& action, HandlerBase* current)
        : setHandler(std::move(action))
    {
        setHandler(current);
    }
    ~SetCurrentHandler()
    {
        setHandler(nullptr);
    }
};

void HandlerBase::activateEventHandlers(LibSocketId sockId, short eventType)
{
    SetCurrentHandler   setCurrentHandler([&parent=this->parent](HandlerBase* current){parent.setCurrentHandler(current);}, this);
    short newEventType = eventActivate(sockId, eventType);
    setHandlers(newEventType);
}

void HandlerBase::setSuspend(HandlerBase& handlerToSuspend)
{
    suspended = &handlerToSuspend;
    handlerToSuspend.suspend(0);
}

void HandlerBase::resume()
{
    setHandlers(EV_READ | EV_WRITE);
}

void HandlerBase::dropHandler()
{
    doDropHandler(true);
}

void HandlerBase::doDropHandler(bool closeStream)
{
    /*
     * Note:
     *      Called from dropHandler() with closeStream = true;
     *      Called from moveHandler() with closeStream = false;
     */
    dropEvent();
    if (closeStream)
    {
        close();
    }
    parent.delHandler(this);
    if (suspended)
    {
        suspended->resume();
    }
}

void HandlerBase::dropEvent()
{
    if (::event_del(readEvent.get()) != 0)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::Core::Service::HandlerBase::dropHandler: readEvent event_del(): Failed");
    }
    if (::event_del(writeEvent.get()) != 0)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::Core::Service::HandlerBase::dropHandler: writeEvent event_del(): Failed");
    }
}

void HandlerBase::setHandlers(short eventType, TimeVal* timeVal)
{
    if (timeVal != nullptr || (eventType & EV_READ))
    {
        if (::event_add(readEvent.get(), timeVal) != 0)
        {
            throw std::runtime_error("ThorsAnvil::Nisse::Core::Service::HandlerBase: readEvent: event_add(): Failed");
        }
    }
    if (eventType & EV_WRITE)
    {
        if (::event_add(writeEvent.get(), timeVal) != 0)
        {
            throw std::runtime_error("ThorsAnvil::Nisse::Core::Service::HandlerBase: writeEvent: event_add(): Failed");
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
#include "ServerHandler.tpp"
#include "test/Action.h"
#include "ThorsNisseCoreSocket/Socket.h"
template void ThorsAnvil::Nisse::Core::Service::Server::listenOn<TestHandler, std::tuple<bool, bool, bool>>(ServerConnection const&, std::tuple<bool, bool, bool>&);
template void ThorsAnvil::Nisse::Core::Service::Server::listenOn<InHandlerTest, std::tuple<bool, std::function<void(ThorsAnvil::Nisse::Core::Service::Server&)>>>(ServerConnection const&, std::tuple<bool, std::function<void(ThorsAnvil::Nisse::Core::Service::Server&)>>&);

template void ThorsAnvil::Nisse::Core::Service::HandlerBase::addHandler<ActionUnReg, ThorsAnvil::Nisse::Core::Socket::DataSocket>(ThorsAnvil::Nisse::Core::Socket::DataSocket&&);
template void ThorsAnvil::Nisse::Core::Service::HandlerBase::addHandler<Action, ThorsAnvil::Nisse::Core::Socket::DataSocket>(ThorsAnvil::Nisse::Core::Socket::DataSocket&&);
template ThorsAnvil::Nisse::Core::Service::ServerHandler<TestHandler, std::tuple<bool, bool, bool> >::ServerHandler(ThorsAnvil::Nisse::Core::Service::Server&, ThorsAnvil::Nisse::Core::Socket::ServerSocket&&, std::tuple<bool, bool, bool>&);
template ThorsAnvil::Nisse::Core::Service::ServerHandler<InHandlerTest, std::tuple<bool, std::function<void (ThorsAnvil::Nisse::Core::Service::Server&)> > >::ServerHandler(ThorsAnvil::Nisse::Core::Service::Server&, ThorsAnvil::Nisse::Core::Socket::ServerSocket&&, std::tuple<bool, std::function<void (ThorsAnvil::Nisse::Core::Service::Server&)> >&);
#endif
