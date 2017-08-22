#ifndef THORSANVIL_NISSE_PROTOCOL_HTTP_DYNAMIC_SITE_LOADER_H
#define THORSANVIL_NISSE_PROTOCOL_HTTP_DYNAMIC_SITE_LOADER_H

#include "Binder.h"
#include "HttpScanner.h"
#include "ThorsNisseCoreService/Server.h"
#include "ThorsNisseCoreService/Handler.h"
#include <map>
#include <string>

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace Protocol
        {
            namespace HTTP
            {

class DynamicSiteLoader
{
    using SiteKey  = std::pair<std::string, std::string>;
    using SiteInfo = std::tuple<void*, int>;

    Core::Service::Server&          server;
    std::map<int, Binder>           portMap;
    std::map<SiteKey, SiteInfo>     loadedLibs;
    public:
        DynamicSiteLoader(Core::Service::Server& server);
        void load(std::string const& site, int port, std::string const& host, std::string const& base);
        void unload(std::string const& host, std::string const& base);
};

class DeveloperHandler: public Core::Service::Handler
{
    DynamicSiteLoader&          loader;
    Core::Socket::DataSocket    socket;
    HttpScanner                 scanner;
    std::vector<char>           buffer;
    public:
        DeveloperHandler(Core::Service::Server& parent, Core::Socket::DataSocket&& socket, DynamicSiteLoader& loader);
        virtual short eventActivate(Core::Service::LibSocketId sockId, short eventType) override;
        virtual bool  blocking()  override {return true;}
};

            }
        }
    }
}

#endif
