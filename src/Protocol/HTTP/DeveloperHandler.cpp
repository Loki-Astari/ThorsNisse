#include "DeveloperHandler.h"
#include "ThorsNisseCoreSocket/SocketStream.h"
#include "ThorSerialize/Traits.h"
#include "ThorSerialize/JsonThor.h"

using namespace ThorsAnvil::Nisse::Protocol::HTTP;

DeveloperHandler::DeveloperHandler(Core::Service::Server& parent, Core::Socket::DataSocket&& socket, DynamicSiteLoader& loader)
    : HandlerNonSuspendable(parent, socket.getSocketId(), EV_READ | EV_WRITE)
    , loader(loader)
    , socket(std::move(socket))
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
        std::tie(more, recved) = socket.getMessageData(&buffer[0], 100, 0);
        scanner.scan(&buffer[0], recved);

        if (scanner.data.messageComplete || !more)
        {
            break;
        }
    }

    Core::Socket::ISocketStream   input(socket,  [](){}, [](){}, std::move(buffer), scanner.data.bodyBegin, scanner.data.bodyEnd);
    Core::Socket::OSocketStream   output(socket, [](){}, [](){});

    LoadSite siteToLoad;
    input >> ThorsAnvil::Serialize::jsonImport(siteToLoad);
    std::cerr << this << ": " << "Got: " << siteToLoad.action << " lib: " << siteToLoad.lib << " Host: " << siteToLoad.host << ":" << siteToLoad.port << "/" << siteToLoad.base << "\n";

    int         status  = 200;
    std::string message = "OK";
    try
    {
        if (siteToLoad.action == "Unload")
        {
            auto result = loader.unload(siteToLoad.host, siteToLoad.base);

            if (!result.first)
            {
                status  = 400;
                message = "Bad Request";
            }
            else if (result.second != 0)
            {
                status  = 205;
                message = "Site Disabled, but calls still active";
            }
        }
        if (siteToLoad.action == "Load")
        {
            loader.load(siteToLoad.lib, siteToLoad.port, siteToLoad.host, siteToLoad.base);
        }
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

    dropHandler();
    return 0;
}
