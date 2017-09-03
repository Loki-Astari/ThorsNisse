#ifndef THORSANVIL_NISSE_CORE_SERVICE_HANDLER_H
#define THORSANVIL_NISSE_CORE_SERVICE_HANDLER_H

#include "EventUtil.h"
#include "CoRoutine.h"
#include "ThorsNisseCoreSocket/Socket.h"
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
        virtual short eventActivate(LibSocketId /*sockId*/, short /*eventType*/) final
        {
            if (worker == nullptr)
            {
                worker.reset(new CoRoutine([&parentYield = this->yield, &parent = *this, firstEvent = this->firstEvent](Yield& yield)
                    {
                        parentYield = &yield;
                        yield(firstEvent);
                        parent.eventActivateNonBlocking();
                        return 0;
                    }));
            }
            if (!(*worker)())
            {
                this->dropHandler();
                return 0;
            }
            return worker->get();
        }
        virtual void eventActivateNonBlocking() = 0;
};

template<typename ActHand, typename Param>
class ServerHandler: public HandlerNonSuspendable<Socket::ServerSocket>
{
    private:
        Param&                  param;
    public:
        ServerHandler(Server& parent, Socket::ServerSocket&& so, Param& param);
        ~ServerHandler();
        virtual short eventActivate(LibSocketId sockId, short eventType) override;
};

template<typename ActHand>
class ServerHandler<ActHand, void>: public HandlerNonSuspendable<Socket::ServerSocket>
{
    public:
        ServerHandler(Server& parent, Socket::ServerSocket&& so);
        ~ServerHandler();
        virtual short eventActivate(LibSocketId sockId, short eventType) override;
};

class TimerHandler: public HandlerNonSuspendable<int>
{
    std::function<void()>        action;
    public:
        TimerHandler(Server& parent, double timeOut, std::function<void()>&& action)
            : HandlerNonSuspendable(parent, -1, EV_PERSIST, timeOut)
            , action(std::move(action))
        {}
        virtual short eventActivate(LibSocketId /*sockId*/, short /*eventType*/) override
        {
            action();
            return 0;
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
