#include "Service.h"
#include "ThorsSocket/SocketStream.h"

using namespace ThorsAnvil::Nisse::Core::Service;
using TimeVal = struct timeval;

//virtual short HandlerBase::eventActivate(LibSocketId sockId, short eventType)
/** MethodDesc:
Method called when there is data on the socket.
@ return Return the type of event that you can next process on this stream: EV_READ or EV_WRITE or (EV_READ | EV_WITE)
@ param sockId The socket with data available on it.
@ param eventType The type of event that caused this handler to be triggered (EV_READ | EV_WRITE)
*/
//virtual bool HandlerBase::suspendable()
/** MethodDesc:
Return true if the handler is suspendable and false otherwise.
It is best to inherit from `HandlerNonSuspendable` or aHandlerNonSuspendable` rather than implement this yourself.
*/
//virtual void HandlerBase::suspend(short type)
/** MethodDesc:
Suspend the execution of this class until `type` (EV_READ | EV_WRITE) is available for this socket.
This basically returns control to the main event loop until data is available. This is used by the stream handlers to return control to the main event loop if they would block when reading from a stream.
*/
//virtual void HandlerBase::close()
/** MethodDesc:
Close the stream associated with this handler.
*/
//virtual bool HandlerSuspendable::eventActivateNonBlocking()
/** MethodDesc:
Called by eventActivate() to start processing of the stream.
If a blocking operation is about to be performed this method should call `suspend()` to return control to the main event loop.
@ return Return true to cause the handler to be re-used.<br>Return false to not drop the handler. This is used if something clever is happening.
*/

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
    /** MethodDesc:
    When a handler is finished processing events on a stream. It can call dropHandler() to remove itself from the event loop.
    This is usually done automatically by higher level derived handlers.
    */
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
        throw std::runtime_error("ThorsAnvil::Nisse::Core::Service::HandlerBase::dropEvent: readEvent event_del(): Failed");
    }
    if (::event_del(writeEvent.get()) != 0)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::Core::Service::HandlerBase::dropEvent: writeEvent event_del(): Failed");
    }
}

void HandlerBase::setHandlers(short eventType, TimeVal* timeVal)
{
    if (timeVal != nullptr || (eventType & EV_READ))
    {
        if (::event_add(readEvent.get(), timeVal) != 0)
        {
            throw std::runtime_error("ThorsAnvil::Nisse::Core::Service::HandlerBase::setHandler: readEvent: event_add(): Failed");
        }
    }
    if (eventType & EV_WRITE)
    {
        if (::event_add(writeEvent.get(), timeVal) != 0)
        {
            throw std::runtime_error("ThorsAnvil::Nisse::Core::Service::HandlerBase::setHandler: writeEvent: event_add(): Failed");
        }
    }
}

bool HandlerSuspendableWithStream::eventActivateNonBlocking()
{
    ThorsSocket::IOSocketStream   ioStream(stream, [&parent = *this](){parent.suspend(EV_READ);}, [&parent = *this](){parent.suspend(EV_WRITE);});
    //Core::Socket::ISocketStream   input(stream,  [&parent = *this](){parent.suspend(EV_READ);},  [](){});
    //Core::Socket::OSocketStream   output(stream, [&parent = *this](){parent.suspend(EV_WRITE);}, [](){});

    return eventActivateWithStream(ioStream, ioStream);
}
