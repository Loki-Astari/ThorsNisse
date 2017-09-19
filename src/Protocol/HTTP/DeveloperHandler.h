#ifndef THORSANVIL_NISSE_PROTOCOL_HTTP_DEVELOPER_HANDLER_H
#define THORSANVIL_NISSE_PROTOCOL_HTTP_DEVELOPER_HANDLER_H

#include "DynamicSiteLoader.h"
#include "HttpScanner.h"
#include "ThorsNisseCoreService/Server.h"
#include "ThorsNisseCoreService/Handler.h"
#include "ThorsNisseCoreService/ServerHandler.h"
#include <vector>

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace Protocol
        {
            namespace HTTP
            {

class DeveloperHandler: public Core::Service::HandlerNonSuspendable<Core::Socket::DataSocket>
{
    DynamicSiteLoader&          loader;
    HttpScanner                 scanner;
    std::vector<char>           buffer;
    public:
        DeveloperHandler(Core::Service::Server& parent, Core::Socket::DataSocket&& socket, DynamicSiteLoader& loader);
        virtual short eventActivate(Core::Service::LibSocketId sockId, short eventType) override;
};

            }
        }
    }
}

#endif
