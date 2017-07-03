#ifndef THORSANVIL_NISSE_NISSE_SERVICE_H
#define THORSANVIL_NISSE_NISSE_SERVICE_H

#include "ThorsSocket/Socket.h"
#include <event.h>

namespace ThorsAnvil
{
    namespace Nisse
    {
using LibEventBase  = struct event_base;
using LibEvent      = struct event;
using LibSocketId   = evutil_socket_t;
    }
}

extern "C" void eventCB(ThorsAnvil::Nisse::LibSocketId sockId, short eventType, void* event);

namespace ThorsAnvil
{
    namespace Nisse
    {

class NisseEvent
{
    LibEvent*   event;
    public:
        ~NisseEvent()
        {
            if (event_del(event) != 0)
            {
                //throw std::runtime_error("ThorsAnvil::Nisse::NisseEvent::NisseEvent: event_del(): Failed");
            }
            event_free(event);
        }
        NisseEvent(LibEventBase* base, LibSocketId socketId)
            : event(event_new(base, socketId,  EV_READ | EV_PERSIST, eventCB, this))
        {
            if (event == nullptr)
            {
                throw std::runtime_error("ThorsAnvil::Nisse::NisseEvent::NisseEvent: event_new(): Failed");
            }
            if (event_add(event, nullptr) != 0)
            {
                event_free(event);
                throw std::runtime_error("ThorsAnvil::Nisse::NisseEvent::NisseEvent: event_add(): Failed");
            }
        }

        NisseEvent(NisseEvent const&)               = delete;
        NisseEvent(NisseEvent&&)                    = delete;
        NisseEvent& operator=(NisseEvent const&)    = delete;
        NisseEvent& operator=(NisseEvent&&)         = delete;

    private:
        friend void ::eventCB(LibSocketId sockId, short eventType, void* event);
        virtual void eventActivate(LibSocketId sockId, short eventType);
};

class ServerEvent
{
    private:
        ThorsAnvil::Socket::ServerSocket    socket;
        NisseEvent                          eventListener;
    public:
        ServerEvent(LibEventBase* base, int port)
            : socket(port)
            , eventListener(base, socket.getSocketId())
        {}
};

class NisseService
{
    private:

        bool            running;
        LibEventBase*   eventBase;
    public:
        ~NisseService();
        NisseService();

        NisseService(NisseService const&)               = delete;
        NisseService(NisseService&&)                    = delete;
        NisseService& operator=(NisseService const&)    = delete;
        NisseService& operator=(NisseService&&)         = delete;

        void start();
        void flagShutDown();

        void listenOn(int port);
    private:
        void runLoop();
};

    }
}

#endif
