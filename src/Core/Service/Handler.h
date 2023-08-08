#ifndef THORSANVIL_NISSE_CORE_SERVICE_HANDLER_H
#define THORSANVIL_NISSE_CORE_SERVICE_HANDLER_H

#ifndef THORSANVIL_NISSE_CORE_SERVICE_SERVICE_H
#error "Please do not include >ThorsNisseCoreService/Handler.h< directly. Include the >ThorsNisseCoreService/Service.h< file instead."
#endif

#include "EventUtil.h"
#include "CoRoutine.h"
#include "ThorsNisseCoreSocket/Socket.h"
#include <memory>
#include <functional>
#include <stdexcept>

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
// @class
class HandlerBase
{
    private:
        Server&                             parent;
        NisseEvent                          readEvent;
        NisseEvent                          writeEvent;
        HandlerBase*                        suspended;

    public:
        // @method
        HandlerBase(Server& parent, LibSocketId socketId, short eventType, double timeout = 0);
        virtual ~HandlerBase();
        void activateEventHandlers(LibSocketId sockId, short eventType);        // The C-Callback point.
                                                                                // Should make this private

        // @method
        // Method called when there is data on the socket.
        // @return          Return the type of event that you can next process on this stream- EV_READ or EV_WRITE or (EV_READ | EV_WITE)
        // @param sockId    The socket with data available on it.
        // @param eventType The type of event that caused this handler to be triggered (EV_READ | EV_WRITE)
        virtual short eventActivate(LibSocketId sockId, short eventType) = 0;
        // @method
        // Return true if the handler is suspendable and false otherwise.
        // It is best to inherit from `HandlerNonSuspendable` or aHandlerNonSuspendable` rather than implement this yourself.
        virtual bool  suspendable() = 0;
        // @method
        // Close the stream associated with this handler.
        virtual void  close() = 0;
        void setHandlers(short eventType, TimeVal* timeVal = nullptr);
    protected:
        // @method
        // When a handler is finished processing events on a stream. It can call dropHandler() to remove itself from the event loop.
        // This is usually done automatically by higher level derived handlers.
        void dropHandler();
        // @method
        // If the handler wants to create other handlers.
        // An example of this is when the SQL handlers are added. They create connections to the SQL server that require their own event handlers.
        // @param args Arguments passed to the constructor of the new handler.
        template<typename H, typename... Args>
        void addHandler(Args&&... args);
        // @method
        // This is similar to `addHandler()`.
        // The difference is that the current handler will immediately suspend until the created handler complets.
        // When the added handler calls `dropHandler()` control will be returned to the current handler at the point it suspended.
        // Note- The current handler must be suspendable (otherwise an exception is thrown).See-  href="HandlerBase::suspendable">HandlerBase::suspendable
        // @param args Arguments passed to the constructor of the new handler
        template<typename H, typename... Args>
        void moveHandler(Args&&... args);
    public:
        void dropEvent();
    private:
        // @method
        // Suspend the execution of this class until `type` (EV_READ | EV_WRITE) is available for this socket.
        // This basically returns control to the main event loop until data is available.
        // This is used by the stream handlers to return control to the main event loop if they would block when reading from a stream.
        // @param type The type of event we are waiting for.
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

// @class
// This class is templatized based on the type of stream the socket represents.
// The class basically defines a common class for holding the stream object and how to close it when required.
template<typename Stream>
class HandlerStream: public HandlerBase
{
    protected:
        Stream      stream;
        using HandlerBase::dropHandler;
    public:
        // @method
        HandlerStream(Server& parent, Stream&& stream, short eventType, double timeout = 0);
        // @method
        virtual void  close() override;
};

// @class
// Defines `suspendable()` and `suspend()` for a class that is non suspendable.
// This is used by some of there server built in handlers that must complete. It is unlikely that this will be useful for a user defined handler.
template<typename Stream>
class HandlerNonSuspendable: public HandlerStream<Stream>
{
    public:
        using HandlerStream<Stream>::HandlerStream;
        // @method
        virtual void suspend(short) final {throw std::runtime_error("ThorsAnvil::Nisse::HandlerNonSuspendable::suspend: Failed");};
        // @method
        virtual bool suspendable()  final {return false;}
};

using CoRoutine = ThorsAnvil::Nisse::Core::Service::Context<short>::pull_type;
using Yield     = ThorsAnvil::Nisse::Core::Service::Context<short>::push_type;

// @class
// Defines a handler that is suspendable.
// Implements `suspendable`, `suspsend()` and `eventActivate()` as these all work together to define a class that can be suspended.
// The method `eventActivateNonBlocking()` should be overridden by derived classes to provide functionally.
template<typename Stream>
class HandlerSuspendable: public HandlerStream<Stream>
{
    Yield*                      yield;
    std::unique_ptr<CoRoutine>  worker;
    short                       firstEvent;
    public:
        // @method
        HandlerSuspendable(Server& parent, Stream&& stream, short eventType);
        // @method
        HandlerSuspendable(Server& parent, Stream&& stream, short eventType, short firstEvent);

        // @method
        virtual void suspend(short type)    final {(*yield)(type);}
        // @method
        virtual bool suspendable()          final {return true;}
        // @method
        // On first call will start the method `eventActivateNonBlocking()`. This method may suspend itself by calling `suspend()`.
        // On subsequent calls will resume `eventActivateNonBlocking()` at the point where `suspend()` was called.
        // If `eventActivateNon Blocking()` returns true then `dropHandler()` is called to remove this handler as processing is complete.
        // @return This method returns the type of socket event that should be listened for in the main event loop.
        virtual short eventActivate(LibSocketId sockId, short eventType) final;
        // @method
        virtual bool eventActivateNonBlocking() = 0;
};

// @class
// An implementation of `eventActivateNonBlocking()` that creates input and output stream objects.
// These stream objects will call `suspend()` if they are about to perform a blocking operation on the underlying socket.
//
// Thus we have transparently non-blocking streams.
class HandlerSuspendableWithStream: public HandlerSuspendable<Socket::DataSocket>
{
    public:
        using HandlerSuspendable::HandlerSuspendable;
        // @method
        virtual bool eventActivateNonBlocking() final;
        // @method
        virtual bool eventActivateWithStream(std::istream& input, std::ostream& output) = 0;
};


            }
        }
    }
}

#endif
