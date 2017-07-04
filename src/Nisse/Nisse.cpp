#include "NisseService.h"
#include "ProtocolSimpleNisse.h"
#include "NisseHandler.h"

#include <iostream>

int main()
{
    try
    {
        using ThorsAnvil::Nisse::NisseService;
        using ThorsAnvil::Nisse::DataHandlerReadMessage;

        NisseService     service;
        service.listenOn<DataHandlerReadMessage>(40715);
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
