#ifndef THORSANVIL_NISSE_NISSE_HANDLER_H
#define THORSANVIL_NISSE_NISSE_HANDLER_H

#include "NisseEventUtil.h"
#include "ThorsNisseSocket/Socket.h"

extern "C" void eventCB(ThorsAnvil::Nisse::LibSocketId socketId, short eventType, void* event);

namespace ThorsAnvil
{
    namespace Nisse
    {

using EventDeleter  = decltype(&event_free);
using NisseEvent    = std::unique_ptr<LibEvent, EventDeleter>;

class NisseService;
class NisseHandler
{
    private:
        NisseService&                       parent;
        NisseEvent                          event;
    public:
        NisseHandler(NisseService& parent, LibSocketId socketId, short eventType, double timeout = 0);
        virtual ~NisseHandler();
        virtual void eventActivate(LibSocketId sockId, short eventType);
    protected:
        void dropHandler();
        template<typename H, typename... Args>
        void addHandler(Args&&... args);
        template<typename H, typename... Args>
        void moveHandler(Args&&... args);
    public:
        void dropEvent();
};

template<typename Handler, typename Param>
class ServerHandler: public NisseHandler
{
    private:
        ThorsAnvil::Socket::ServerSocket    socket;
        Param&                              param;
    public:
        ServerHandler(NisseService& parent, ThorsAnvil::Socket::ServerSocket&& so, Param& param);
        ~ServerHandler();
        virtual void eventActivate(LibSocketId sockId, short eventType) override;
};

template<typename Handler>
class ServerHandler<Handler, void>: public NisseHandler
{
    private:
        ThorsAnvil::Socket::ServerSocket    socket;
    public:
        ServerHandler(NisseService& parent, ThorsAnvil::Socket::ServerSocket&& so);
        ~ServerHandler();
        virtual void eventActivate(LibSocketId sockId, short eventType) override;
};

template<typename Func>
class TimerHandler: public NisseHandler
{
    Func        action;
    public:
        TimerHandler(NisseService& parent, double timeOut, Func&& action)
            : NisseHandler(parent, -1, EV_PERSIST, timeOut)
            , action(std::move(action))
        {}
        virtual void eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
        {
            action();
        }
};

    }
}

#ifndef COVERAGE_TEST
#include "NisseHandler.tpp"
#endif
#endif
