#include "NisseService.h"

#include <iostream>

int main()
{
    try
    {
        ThorsAnvil::Nisse::NisseService     service;
        service.listenOn(40715);
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
