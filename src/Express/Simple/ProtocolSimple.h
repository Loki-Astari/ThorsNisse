#ifndef THORSANVIL_NISSE_PROTOCOL_SIMPLE_PROTOCOL_SIMPLE_H
#define THORSANVIL_NISSE_PROTOCOL_SIMPLE_PROTOCOL_SIMPLE_H

#include "ThorsNisseCoreService/Handler.h"
#include "ThorsNisseCoreService/ServerHandler.h"

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace Protocol
        {
            namespace Simple
            {

class ReadMessageHandler: public Core::Service::HandlerNonSuspendable<Core::Socket::DataSocket>
{
    private:
        std::size_t                         readSizeObject;
        std::size_t                         readBuffer;
        std::size_t                         bufferSize;
        std::string                         buffer;
    public:
        ReadMessageHandler(Core::Service::Server& parent, Core::Socket::DataSocket&& socket);
        virtual short eventActivate(Core::Service::LibSocketId sockId, short eventType) override;

    public:
        static std::string const failSizeMessage;
        static std::string const failIncompleteMessage;
};

class WriteMessageHandler: public Core::Service::HandlerNonSuspendable<Core::Socket::DataSocket>
{
    private:
        std::size_t                         writeSizeObject;
        std::size_t                         writeBuffer;
        std::string                         message;
    public:
        WriteMessageHandler(Core::Service::Server& parent, Core::Socket::DataSocket&& socket, std::string const& message, bool ok = false);
        virtual short eventActivate(Core::Service::LibSocketId sockId, short eventType) override;
    public:
        static std::string const messageSuffix;
};

            }
        }
    }
}

#endif
