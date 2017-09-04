#include "ThorsNisseCoreService/Server.h"
#include "ThorsNisseCoreService/Handler.h"
#include "ThorsNisseProtocolSimple/ProtocolSimple.h"
#include "ThorsNisseProtocolSimple/ProtocolSimpleStream.h"
#include "ThorsNisseProtocolHTTP/HTTPProtocol.h"
#include "ThorsNisseProtocolHTTP/DynamicSiteLoader.h"
#include "ThorsNisseProtocolHTTP/DeveloperHandler.h"
#include "ThorsNisseProtocolHTTP/Types.h"
#include <sstream>

#include <iostream>

namespace Nisse = ThorsAnvil::Nisse::Core::Service;
namespace HTTP  = ThorsAnvil::Nisse::Protocol::HTTP;

struct LoadSite
{
    std::string     lib;
    std::string     host;
    std::string     base;
    int             port;

    friend std::istream& operator>>(std::istream& str, LoadSite& site)
    {
        std::getline(str, site.lib, ':');
        std::getline(str, site.host, ':');
        std::getline(str, site.base, ':');
        str >> site.port;
        return str;
    }
};

int main(int argc, char* argv[])
{
    try
    {
        Nisse::Server           server;
        HTTP::DynamicSiteLoader siteLoader(server);

        std::vector<std::string>    args(argv + 1, argv + argc);
        for (std::string const& arg: args)
        {
            if (arg.substr(0,7) == "--wait:")
            {
                int maxWaitingConnections = std::stoi(arg.substr(7));
                siteLoader.setMaxWaitingConnections(maxWaitingConnections);
            }
            if (arg.substr(0,7) == "--load:")
            {
                LoadSite            site;
                std::stringstream   loadStream(arg.substr(7));
                loadStream >> site;

                std::cerr << "Loading: " << site.lib << " On: " << site.host << ":" << site.port << "/" << site.base << "\n";
                siteLoader.load(site.lib, site.port, site.host, site.base);
            }
            if (arg.substr(0,8)  == "--debug:")
            {
                int port = std::stoi(arg.substr(8));
                std::cerr << "Debugger: " << port << "\n";
                server.listenOn<HTTP::DeveloperHandler>(port, siteLoader);
            }
        }

        /*
        using ThorsAnvil::Nisse::ProtocolSimple::ReadMessageHandler;
        service.listenOn<ReadMessageHandler>(40717);

        using ThorsAnvil::Nisse::ProtocolSimple::ReadMessageStreamHandler;
        service.listenOn<ReadMessageStreamHandler>(40718);
        */

        server.start();
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
