#include "NisseService.h"
#include "ProtocolSimpleNisse.h"
#include "ProtocolHTTPNisse.h"
#include "HttpBinder.h"
#include "HttpTypes.h"
#include "NisseHandler.h"

#include <iostream>

int main()
{
    try
    {
        using ThorsAnvil::Nisse::NisseService;
        using ThorsAnvil::Nisse::HTTPHandlerAccept;
        using ThorsAnvil::Nisse::HTTPBinder;
        using ThorsAnvil::Nisse::HTTPRequest;
        using ThorsAnvil::Nisse::HTTPResponse;

        NisseService    service;
        HTTPBinder      binder;

        binder.add("/listBeer",[](HTTPRequest& /*request*/, HTTPResponse& /*response*/){std::cerr << "ListBeer\n";});
        binder.add("/addBeer", [](HTTPRequest& /*request*/, HTTPResponse&/* response*/){std::cerr << "AddBeer\n";});

        service.listenOn<HTTPHandlerAccept>(40716, binder);

        using ThorsAnvil::Nisse::DataHandlerReadMessage;
        service.listenOn<DataHandlerReadMessage>(40717);

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
