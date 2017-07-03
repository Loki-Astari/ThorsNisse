#ifndef THORSANVIL_NISSE_NISSE_HANDLER_H
#define THORSANVIL_NISSE_NISSE_HANDLER_H

#include "NisseEventUtil.h"
#include "ThorsSocket/Socket.h"

extern "C" void eventCB(ThorsAnvil::Nisse::LibSocketId socketId, short eventType, void* event);

namespace ThorsAnvil
{
    namespace Nisse
    {

class NisseEvent
{
    LibEvent*   event;
    public:
        ~NisseEvent();
        NisseEvent(LibEventBase* base, LibSocketId socketId);

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
        ServerEvent(LibEventBase* base, int port);
};

    }
}

#endif
