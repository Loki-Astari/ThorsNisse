#ifndef THORSANVIL_NISSE_PROTOCOL_SIMPLE_NISSE_H
#define THORSANVIL_NISSE_PROTOCOL_SIMPLE_NISSE_H

#include "NisseHandler.h"

namespace ThorsAnvil
{
    namespace Nisse
    {

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
