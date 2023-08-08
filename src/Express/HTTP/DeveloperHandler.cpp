#include "DeveloperHandler.h"
#include "ThorsSocket/SocketStream.h"
#include "ThorSerialize/Traits.h"
#include "ThorSerialize/JsonThor.h"
#include <iostream>

using namespace ThorsAnvil::Nisse::Protocol::HTTP;

DeveloperHandler::DeveloperHandler(Core::Service::Server& parent, ThorsSocket::DataSocket&& socket, DynamicSiteLoader& loader)
    : HandlerNonSuspendable(parent, std::move(socket), EV_READ | EV_WRITE)
    , loader(loader)
    , buffer(100)
{}

struct LoadSite
{
    std::string     action;
    std::string     lib;
    std::string     host;
    std::string     base;
    int             port;
};

ThorsAnvil_MakeTrait(LoadSite, action, lib, host, base, port);

short DeveloperHandler::eventActivate(Core::Service::LibSocketId, short)
{
    while (!scanner.data.messageComplete)
    {
        bool                more;
        std::size_t         recved;
        std::tie(more, recved) = stream.getMessageData(&buffer[0], 100, 0);
        scanner.scan(&buffer[0], recved);

        if (scanner.data.messageComplete || !more)
        {
            break;
        }
    }

    ThorsSocket::IOSocketStream   input(stream,  [](){}, [](){}, std::move(buffer), scanner.data.bodyBegin, scanner.data.bodyEnd);
    ThorsSocket::IOSocketStream   output(stream, [](){}, [](){});

    int         status  = 200;
    std::string message = "OK";
    try
    {
        LoadSite siteToLoad;

        input >> ThorsAnvil::Serialize::jsonImporter(siteToLoad);
        std::cerr << this << ": " << "Got: " << siteToLoad.action << " lib: " << siteToLoad.lib << " Host: " << siteToLoad.host << ":" << siteToLoad.port << "/" << siteToLoad.base << "\n";

        if (siteToLoad.action == "Unload")
        {
            auto result = loader.unload(siteToLoad.port, siteToLoad.host, siteToLoad.base);

            if (!std::get<0>(result))
            {
                status  = 400;
                message = "Bad Request";
            }
            else if (std::get<1>(result) != 0)
            {
                status  = 206;
                message = "Site Disabled, but calls still active";
            }
            else if (std::get<2>(result) != 0)
            {
                status  = 206;
                message = "Site Removed, but other sites are still using the lib";
            }
        }
        if (siteToLoad.action == "Load")
        {
            auto result = loader.load(siteToLoad.lib, siteToLoad.port, siteToLoad.host, siteToLoad.base);

            if (!std::get<0>(result))
            {
                status  = 400;
                message = "Bad Request: host:port/base Already loaded";
            }
            else if (std::get<1>(result) != 1)
            {
                status  = 202;
                message = "Site Loaded: Library being re-used";
            }
            else
            {
                status  = 201;
            }
        }
    }
    catch (std::domain_error const& e)
    {
        status  = 500;
        message = "Internal Server Error";
        std::cerr << "Exception: " << e.what() << "\n";
    }
    catch (std::exception const& e)
    {
        status  = 400;
        message = "Bad Request";
        std::cerr << "Exception: " << e.what() << "\n";
    }
    catch (...)
    {
        status  = 500;
        message = "Internal Server Error";
        std::cerr << "Exception: Unknown\n";
    }

    output << "HTTP/1.1 " << status << " " << message << "\r\n"
           << "Content-Length: 0\r\n"
           << "\r\n";

    output.flush();
    dropHandler();
    return 0;
}
