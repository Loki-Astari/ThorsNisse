#ifndef THORSANVIL_NISSE_PROTOCOL_SIMPLE_HANDLERS_H
#define THORSANVIL_NISSE_PROTOCOL_SIMPLE_HANDLERS_H

#include "ThorsNisseCoreService/NisseHandler.h"

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace ProtocolSimple
        {

class ReadMessageHandler: public Core::Service::NisseHandler
{
    private:
        ThorsAnvil::Socket::DataSocket      socket;
        std::size_t                         readSizeObject;
        std::size_t                         readBuffer;
        std::size_t                         bufferSize;
        std::string                         buffer;
    public:
        ReadMessageHandler(Core::Service::NisseService& parent, Socket::DataSocket&& socket);
        virtual short eventActivate(Core::Service::LibSocketId sockId, short eventType) override;

    public:
        static std::string const failSizeMessage;
        static std::string const failIncompleteMessage;
};

class WriteMessageHandler: public Core::Service::NisseHandler
{
    private:
        ThorsAnvil::Socket::DataSocket      socket;
        std::size_t                         writeSizeObject;
        std::size_t                         writeBuffer;
        std::string                         message;
    public:
        WriteMessageHandler(Core::Service::NisseService& parent, Socket::DataSocket&& socket, std::string const& message, bool ok = false);
        virtual short eventActivate(Core::Service::LibSocketId sockId, short eventType) override;
    public:
        static std::string const messageSuffix;
};

        }
    }
}

#endif
