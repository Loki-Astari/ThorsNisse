#ifndef THORSANVIL_NISSE_CORE_SERVICE_HANDLER_H
#define THORSANVIL_NISSE_CORE_SERVICE_HANDLER_H

#include "EventUtil.h"
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

using EventDeleter  = decltype(&event_free);
using NisseEvent    = std::unique_ptr<LibEvent, EventDeleter>;

class Server;
class Handler
{
    private:
        Server&                             parent;
        NisseEvent                          readEvent;
        NisseEvent                          writeEvent;
        Handler*                            suspended;

    public:
        Handler(Server& parent, LibSocketId socketId, short eventType, double timeout = 0);
        virtual ~Handler();
        void activateEventHandlers(LibSocketId sockId, short eventType);
        virtual short eventActivate(LibSocketId sockId, short eventType);
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
        virtual void suspend();
        void resume();
    private:
        friend class Server;
        void setSuspend(Handler& handlerToSuspend);
};

template<typename ActHand, typename Param>
class ServerHandler: public Handler
{
    private:
        Socket::ServerSocket    socket;
        Param&                  param;
    public:
        ServerHandler(Server& parent, Socket::ServerSocket&& so, Param& param);
        ~ServerHandler();
        virtual short eventActivate(LibSocketId sockId, short eventType) override;
};

template<typename ActHand>
class ServerHandler<ActHand, void>: public Handler
{
    private:
        Socket::ServerSocket    socket;
    public:
        ServerHandler(Server& parent, Socket::ServerSocket&& so);
        ~ServerHandler();
        virtual short eventActivate(LibSocketId sockId, short eventType) override;
};

class TimerHandler: public Handler
{
    std::function<void()>        action;
    public:
        TimerHandler(Server& parent, double timeOut, std::function<void()>&& action)
            : Handler(parent, -1, EV_PERSIST, timeOut)
            , action(std::move(action))
        {}
        virtual short eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
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
