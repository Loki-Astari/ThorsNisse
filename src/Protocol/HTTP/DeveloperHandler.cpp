#include "DeveloperHandler.h"
#include "ThorsNisseCoreSocket/SocketStream.h"
#if HAVE_DEVLOADER
#include "ThorSerialize/Traits.h"
#include "ThorSerialize/JsonThor.h"
#endif
#include <iostream>

using namespace ThorsAnvil::Nisse::Protocol::HTTP;

DeveloperHandler::DeveloperHandler(Core::Service::Server& parent, Core::Socket::DataSocket&& socket, DynamicSiteLoader& loader)
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

#if HAVE_DEVLOADER
ThorsAnvil_MakeTrait(LoadSite, action, lib, host, base, port);
#endif

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

    Core::Socket::ISocketStream   input(stream,  [](){}, [](){}, std::move(buffer), scanner.data.bodyBegin, scanner.data.bodyEnd);
    Core::Socket::OSocketStream   output(stream, [](){}, [](){});

    int         status  = 200;
    std::string message = "OK";
#if HAVE_DEVLOADER
    try
    {
        LoadSite siteToLoad;

        input >> ThorsAnvil::Serialize::jsonImport(siteToLoad);
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
#else
    try
    {
        throw std::domain_error("Serialize Not installed. Thus functionality not enabled");
    }
#endif
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

#ifdef COVERAGE_TEST
/*
 * This code is only compiled into the unit tests for code coverage purposes
 * It is not part of the live code.
 */
#include "ThorsNisseCoreService/Server.tpp"
#include "ThorsNisseCoreService/Handler.tpp"
#include "ThorsNisseCoreService/ServerHandler.tpp"
#if HAVE_DEVLOADER
#include "ThorSerialize/Serialize.tpp"
#endif
template void ThorsAnvil::Nisse::Core::Service::Server::listenOn<ThorsAnvil::Nisse::Protocol::HTTP::DeveloperHandler, ThorsAnvil::Nisse::Protocol::HTTP::DynamicSiteLoader>(ServerConnection const&, ThorsAnvil::Nisse::Protocol::HTTP::DynamicSiteLoader&);
template ThorsAnvil::Nisse::Core::Service::ServerHandler<ThorsAnvil::Nisse::Protocol::HTTP::DeveloperHandler, ThorsAnvil::Nisse::Protocol::HTTP::DynamicSiteLoader>::ServerHandler(ThorsAnvil::Nisse::Core::Service::Server&, ThorsAnvil::Nisse::Core::Socket::ServerSocket&&, ThorsAnvil::Nisse::Protocol::HTTP::DynamicSiteLoader&);
#endif
