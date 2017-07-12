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
        namespace Nisse = ThorsAnvil::Nisse;
        namespace HTTP  = ThorsAnvil::Nisse::ProtocolHTTP;

        HTTP::Site      site;
        site.get("/listBeer",[](HTTP::Request& /*request*/, HTTP::Response& response){response.body << "<html><head><title>Beer List</title></head><body><h1>Beer List</h1></body></html>";});
        site.post("/addBeer", [](HTTP::Request& /*request*/, HTTP::Response& /*response*/){std::cerr << "AddBeer\n";});

        HTTP::Binder    binder;
        binder.addSite("test.com", std::move(site));

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
