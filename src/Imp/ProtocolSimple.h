#ifndef THORSANVIL_NISSE_PROTOCOL_SIMPLE_HANDLERS_H
#define THORSANVIL_NISSE_PROTOCOL_SIMPLE_HANDLERS_H

#include "ThorsNisse/NisseHandler.h"

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace ProtocolSimple
        {

class ReadMessageHandler: public NisseHandler
{
    private:
        ThorsAnvil::Socket::DataSocket      socket;
        std::size_t                         readSizeObject;
        std::size_t                         readBuffer;
        std::size_t                         bufferSize;
        std::string                         buffer;
    public:
        ReadMessageHandler(NisseService& parent, ThorsAnvil::Socket::DataSocket&& socket);
        virtual void eventActivate(LibSocketId sockId, short eventType) override;

    public:
        static std::string const failSizeMessage;
        static std::string const failIncompleteMessage;
};

class WriteMessageHandler: public NisseHandler
{
    private:
        ThorsAnvil::Socket::DataSocket      socket;
        std::size_t                         writeSizeObject;
        std::size_t                         writeBuffer;
        std::string                         message;
    public:
        WriteMessageHandler(NisseService& parent, ThorsAnvil::Socket::DataSocket&& socket, std::string const& message, bool ok = false);
        virtual void eventActivate(LibSocketId sockId, short eventType) override;
    public:
        static std::string const messageSuffix;
};

        }
    }
}

#endif
