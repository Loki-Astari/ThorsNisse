#include "ThorsNisseCoreService/Server.h"
#include "ThorsNisseCoreService/Handler.h"
#include "ThorsNisseProtocolSimple/ProtocolSimple.h"
#include "ThorsNisseProtocolSimple/ProtocolSimpleStream.h"
#include "ThorsNisseProtocolHTTP/HTTPProtocol.h"
#include "ThorsNisseProtocolHTTP/DynamicSiteLoader.h"
#include "ThorsNisseProtocolHTTP/Types.h"

#include <iostream>

namespace Nisse = ThorsAnvil::Nisse::Core::Service;
namespace HTTP  = ThorsAnvil::Nisse::Protocol::HTTP;

int main()
{
    try
    {
        Nisse::Server    server;

        HTTP::DynamicSiteLoader siteLoader(server);
        siteLoader.load("../AddBeer/AddBeer.dylib", 40716, "test.com", "");

        /*
        using ThorsAnvil::Nisse::ProtocolSimple::ReadMessageHandler;
        service.listenOn<ReadMessageHandler>(40717);

        using ThorsAnvil::Nisse::ProtocolSimple::ReadMessageStreamHandler;
        service.listenOn<ReadMessageStreamHandler>(40718);
        */

        server.start();
    }
    catch (std::exception const& e)
    {
        //Log exception
        throw;
    }
    catch (...)
    {
        //Log exception
        throw;
    }
}
