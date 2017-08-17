#include "ThorsNisseCoreService/NisseService.h"
#include "ThorsNisseCoreService/NisseHandler.h"
#include "ThorsNisseProtocolSimple/ProtocolSimple.h"
#include "ThorsNisseProtocolSimple/ProtocolSimpleStream.h"
#include "ThorsNisseProtocolHTTP/HTTPProtocol.h"
#include "ThorsNisseProtocolHTTP/Binder.h"
#include "ThorsNisseProtocolHTTP/Types.h"
#include "ThorSQL/Connection.h"
#include "ThorSQL/Statement.h"

#include <iostream>

namespace Nisse = ThorsAnvil::Nisse::Core::Service;
namespace HTTP  = ThorsAnvil::Nisse::Protocol::HTTP;
namespace SQL   = ThorsAnvil::SQL;

int main()
{
    try
    {
        HTTP::Binder    binder;
        binder.load("../AddBeer/AddBeer.dylib");

        Nisse::NisseService    service;
        service.listenOn<HTTP::ReadRequestHandler>(40716, binder);

        /*
        using ThorsAnvil::Nisse::ProtocolSimple::ReadMessageHandler;
        service.listenOn<ReadMessageHandler>(40717);

        using ThorsAnvil::Nisse::ProtocolSimple::ReadMessageStreamHandler;
        service.listenOn<ReadMessageStreamHandler>(40718);
        */

        service.start();
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
