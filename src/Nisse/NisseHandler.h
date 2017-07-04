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
        NisseHandler(NisseService& parent, LibEventBase* base, LibSocketId socketId);
        virtual ~NisseHandler() {}
        virtual void eventActivate(LibSocketId sockId, short eventType);
};

class ServerHandler: public NisseHandler
{
    private:
        ThorsAnvil::Socket::ServerSocket    socket;
    public:
        ServerHandler(NisseService& parent, LibEventBase* base, ThorsAnvil::Socket::ServerSocket&& socket);
        virtual void eventActivate(LibSocketId sockId, short eventType) override;
};

class DataHandlerReadMessage: public NisseHandler
{
    private:
        ThorsAnvil::Socket::DataSocket      socket;
        std::size_t                         readSizeObject;
        std::size_t                         readBuffer;
        std::size_t                         bufferSize;
        std::string                         buffer;
    public:
        DataHandlerReadMessage(NisseService& parent, LibEventBase* base, ThorsAnvil::Socket::DataSocket&& socket);
        virtual void eventActivate(LibSocketId sockId, short eventType) override;
};

class DataHandlerWriteMessage: public NisseHandler
{
    private:
        ThorsAnvil::Socket::DataSocket      socket;
        std::size_t                         writeSizeObject;
        std::size_t                         writeBuffer;
        std::string                         message;
    public:
        DataHandlerWriteMessage(NisseService& parent, LibEventBase* base, ThorsAnvil::Socket::DataSocket&& socket, std::string const& message);
        virtual void eventActivate(LibSocketId sockId, short eventType) override;
};

    }
}

#endif
