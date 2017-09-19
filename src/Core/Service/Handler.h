#ifndef THORSANVIL_NISSE_CORE_SERVICE_HANDLER_H
#define THORSANVIL_NISSE_CORE_SERVICE_HANDLER_H

#include "EventUtil.h"
#include "CoRoutine.h"
#include "ThorsNisseCoreSocket/Socket.h"
#include "ThorsNisseCoreSocket/SocketStream.h"
#include <memory>
#include <functional>

extern "C" void eventCB(ThorsAnvil::Nisse::Core::Service::LibSocketId socketId, short eventType, void* event);

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace Core
        {
            namespace Service
            {

using EventDeleter  = std::function<void(LibEvent*)>;
using NisseEvent    = std::unique_ptr<LibEvent, EventDeleter>;

class Server;
class HandlerBase
{
    /** ClassDesc:
    @ public constructor HandlerBase(Server& parent, LibSocketId socketId, short eventType, double timeout = 0)
    @ protected method void dropHandler()
    @ protected method void addHandler(Args&&... args)
    @ protected method void moveHandler(Args&&... args)
    @ virtual method short eventActivate(LibSocketId sockId, short eventType)
    @ virtual method bool suspendable()
    @ virtual method void suspend(short type)
    @ virtual method void close()
    */
    private:
        Server&                             parent;
        NisseEvent                          readEvent;
        NisseEvent                          writeEvent;
        HandlerBase*                        suspended;

    public:
        HandlerBase(Server& parent, LibSocketId socketId, short eventType, double timeout = 0);
        virtual ~HandlerBase();
        void activateEventHandlers(LibSocketId sockId, short eventType);        // The C-Callback point.
                                                                                // Should make this private
        virtual short eventActivate(LibSocketId sockId, short eventType) = 0;
        virtual bool  suspendable() = 0;
        virtual void  close() = 0;
        void setHandlers(short eventType, TimeVal* timeVal = nullptr);
    protected:
        void dropHandler();
        template<typename H, typename... Args>
        void addHandler(Args&&... args);
        template<typename H, typename... Args>
        void moveHandler(Args&&... args);
    public:
        void dropEvent();
    private:
        virtual void suspend(short type) = 0;
        void resume();
    private:
        friend class Server;
        void setSuspend(HandlerBase& handlerToSuspend);
        void doDropHandler(bool closeStream);
};

template<typename Stream>
inline int getSocketId(Stream& stream)     {return stream.getSocketId();}
template<>
inline int getSocketId<int>(int& value)    {return value;}

template<typename Stream>
inline void closeStream(Stream& stream)    {stream.close();}
template<>
inline void closeStream<int>(int&)         {/*Ignore*/}

template<typename Stream>
class HandlerStream: public HandlerBase
{
    /** ClassDesc:
    This class is templatized based on the type of stream the socket represents.
    The class basically defines a common class for holding the stream object and how to close it when requried.
    @ public constructor HandlerStream(Server& parent, LibSocketId socketId, short eventType)
    @ public method void close()
    */
    protected:
        Stream      stream;
        using HandlerBase::dropHandler;
    public:
        HandlerStream(Server& parent, Stream&& stream, short eventType, double timeout = 0)
            : HandlerBase(parent, getSocketId(stream), eventType, timeout)
            , stream(std::move(stream))
        {}
        virtual void  close() override
        {
            closeStream(stream);
        }
};

template<typename Stream>
class HandlerNonSuspendable: public HandlerStream<Stream>
{
    /** ClassDesc:
    Defines `suspendable()` and `suspend()` for a class that is non suspendable.
    This is used by some of ther server built in handlers that must complete. It is unlikely that this will be useful for a user defined handler.
    @ public constructor HandlerNonSuspendable(Server& parent, LibSocketId socketId, short eventType)
    @ public method bool suspendable()
    @ public method void suspend(short type)
    */
    public:
        using HandlerStream<Stream>::HandlerStream;
        virtual void suspend(short) final {throw std::runtime_error("ThorsAnvil::Nisse::HandlerNonSuspendable::suspend: Failed");};
        virtual bool suspendable()  final {return false;}
};

using CoRoutine = ThorsAnvil::Nisse::Core::Service::Context<short>::pull_type;
using Yield     = ThorsAnvil::Nisse::Core::Service::Context<short>::push_type;

template<typename Stream>
class HandlerSuspendable: public HandlerStream<Stream>
{
    /** ClassDesc:
    Defines a handler that is suspendable.
    Implements `suspendable`, `suspsend()` and `eventActivate()` as these all work together to define a class that can be suspended.
    The method `eventActivateNonBlocking()` should be overwridden by derived classes to provide functionaliy.
    @ public constructor HandlerSuspendable(Server& parent, LibSocketId socketId, short eventType)
    @ public method bool suspendable()
    @ public method void suspend(short type)
    @ public method short eventActivate(LibSocketId sockId, short eventType)
    @ virtual method bool eventActivateNonBlocking()
    */
    Yield*                      yield;
    std::unique_ptr<CoRoutine>  worker;
    short                       firstEvent;
    public:
        HandlerSuspendable(Server& parent, Stream&& stream, short eventType)
            : HandlerSuspendable(parent, std::move(stream), eventType, eventType)
        {}
        HandlerSuspendable(Server& parent, Stream&& stream, short eventType, short firstEvent)
            : HandlerStream<Stream>(parent, std::move(stream), eventType, 0)
            , yield(nullptr)
            , firstEvent(firstEvent)
        {}
        virtual void suspend(short type)    final {(*yield)(type);}
        virtual bool suspendable()          final {return true;}
        virtual short eventActivate(LibSocketId sockId, short eventType) final;
        virtual bool eventActivateNonBlocking() = 0;
};

class HandlerSuspendableWithStream: public HandlerSuspendable<Socket::DataSocket>
{
    /** ClassDesc:
    An implementation of `eventActivateNonBlocking()` that creates input and output stream objects.
    These stream objects will call `suspend()` if they are about to perform a blocking operation on the underlying socket.

    Thus we have transparently non-blocking streams.
    @ public constructor HandlerSuspendable(Server& parent, LibSocketId socketId, short eventType)
    @ public  method bool eventActivateNonBlocking()
    @ virtual method bool eventActivateWithStream(std::istream& input, std::ostream& output)
    */
    public:
        using HandlerSuspendable::HandlerSuspendable;
        virtual bool eventActivateWithStream(std::istream& input, std::ostream& output) = 0;
        virtual bool eventActivateNonBlocking() final
        {
            Core::Socket::ISocketStream   input(stream,  [&parent = *this](){parent.suspend(EV_READ);},  [](){});
            Core::Socket::OSocketStream   output(stream, [&parent = *this](){parent.suspend(EV_WRITE);}, [](){});

            return eventActivateWithStream(input, output);
        }
};


            }
        }
    }
}

#ifndef COVERAGE_TEST
#include "Handler.tpp"
#endif
#endif
