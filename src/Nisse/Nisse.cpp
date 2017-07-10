#include "NisseService.h"
#include "ProtocolSimpleNisse.h"
#include "ProtocolSimpleStreamNisse.h"
#include "ProtocolHTTPNisse.h"
#include "ProtocolHTTPBinder.h"
#include "ProtocolHTTPTypes.h"
#include "NisseHandler.h"

#include <iostream>

int main()
{
    try
    {
        using ThorsAnvil::Nisse::NisseService;
        using ThorsAnvil::Nisse::ProtocolHTTP::Binder;
        using ThorsAnvil::Nisse::ProtocolHTTP::Request;
        using ThorsAnvil::Nisse::ProtocolHTTP::Response;

        NisseService    service;

        Binder          binder;
        binder.get("/listBeer",[](Request& /*request*/, Response& /*response*/){std::cerr << "ListBeer\n";});
        binder.post("/addBeer", [](Request& /*request*/, Response&/* response*/){std::cerr << "AddBeer\n";});

        using ThorsAnvil::Nisse::ProtocolHTTP::ReadRequestHandler;
        service.listenOn<ReadRequestHandler>(40716, binder);

        using ThorsAnvil::Nisse::ProtocolSimple::ReadMessageHandler;
        service.listenOn<ReadMessageHandler>(40717);

        using ThorsAnvil::Nisse::ProtocolSimple::ReadMessageStreamHandler;
        service.listenOn<ReadMessageStreamHandler>(40718);

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
