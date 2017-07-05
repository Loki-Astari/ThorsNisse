#include "NisseService.h"
#include "ProtocolHTTPNisse.h"
#include "NisseHandler.h"

#include <iostream>

int main()
{
    try
    {
        using ThorsAnvil::Nisse::NisseService;
        using ThorsAnvil::Nisse::HTTPHandlerAccept;

        NisseService     service;
        service.listenOn<HTTPHandlerAccept>(40715);
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
