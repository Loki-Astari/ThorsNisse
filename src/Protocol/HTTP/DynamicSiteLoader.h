#ifndef THORSANVIL_NISSE_PROTOCOL_HTTP_DYNAMIC_SITE_LOADER_H
#define THORSANVIL_NISSE_PROTOCOL_HTTP_DYNAMIC_SITE_LOADER_H

#include "Binder.h"
#include "ThorsNisseCoreService/Server.h"
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

            }
        }
    }
}

#endif