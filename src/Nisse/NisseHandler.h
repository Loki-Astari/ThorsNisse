#ifndef THORSANVIL_NISSE_NISSE_HANDLER_H
#define THORSANVIL_NISSE_NISSE_HANDLER_H

#include "NisseEventUtil.h"
#include "ThorsSocket/Socket.h"

extern "C" void eventCB(ThorsAnvil::Nisse::LibSocketId socketId, short eventType, void* event);

namespace ThorsAnvil
{
    namespace Nisse
    {

class NisseService;
class NisseHandler;
class NisseEvent
{
    LibEvent*   event;
    public:
        ~NisseEvent();
        NisseEvent(LibEventBase* base, LibSocketId socketId, NisseHandler& parent, short eventType);

        NisseEvent(NisseEvent const&)               = delete;
        NisseEvent(NisseEvent&&)                    = delete;
        NisseEvent& operator=(NisseEvent const&)    = delete;
        NisseEvent& operator=(NisseEvent&&)         = delete;

        void drop();
};

class NisseHandler
{
    protected:
        NisseService&                       parent;
        NisseEvent                          eventListener;
    public:
        NisseHandler(NisseService& parent, LibEventBase* base, LibSocketId socketId, short eventType);
        virtual ~NisseHandler() {}
        virtual void eventActivate(LibSocketId sockId, short eventType);
};

template<typename Handler>
class ServerHandler: public NisseHandler
{
    private:
        ThorsAnvil::Socket::ServerSocket    socket;
    public:
        ServerHandler(NisseService& parent, LibEventBase* base, ThorsAnvil::Socket::ServerSocket&& so);
        virtual void eventActivate(LibSocketId /*sockId*/, short /*eventType*/) override;
};

    }
}

#ifndef COVERAGE_TEST
#include "NisseHandler.tpp"
#endif
#endif
