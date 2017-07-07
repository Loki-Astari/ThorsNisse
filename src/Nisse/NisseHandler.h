#ifndef THORSANVIL_NISSE_NISSE_HANDLER_H
#define THORSANVIL_NISSE_NISSE_HANDLER_H

#include "NisseEventUtil.h"
#include "ThorsSocket/Socket.h"

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
        NisseHandler(NisseService& parent, LibEventBase* base, LibSocketId socketId, short eventType);
        virtual ~NisseHandler() {}
        virtual void eventActivate(LibSocketId sockId, short eventType);
    protected:
        void dropHandler();
        template<typename H, typename... Args>
        void addHandler(Args&&... args);
        template<typename H, typename... Args>
        void moveHandler(Args&&... args);
};

template<typename Handler>
class ServerHandler: public NisseHandler
{
    private:
        ThorsAnvil::Socket::ServerSocket    socket;
    public:
        ServerHandler(NisseService& parent, LibEventBase* base, ThorsAnvil::Socket::ServerSocket&& so);
        virtual void eventActivate(LibSocketId sockId, short eventType) override;
};

    }
}

#ifndef COVERAGE_TEST
#include "NisseHandler.tpp"
#endif
#endif
