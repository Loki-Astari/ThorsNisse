#ifndef THORSANVIL_NISSE_PROTOCOL_HTTP_DYNAMIC_SITE_LOADER_H
#define THORSANVIL_NISSE_PROTOCOL_HTTP_DYNAMIC_SITE_LOADER_H

#include "Binder.h"
#include "ThorsNisseCoreService/Service.h"
#include <map>
#include <string>
#include <tuple>
#include <utility>

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace Protocol
        {
            namespace HTTP
            {

// @class
class DynamicSiteLoader
{
    using SiteKey  = std::tuple<std::string, std::string, int>;
    using SiteInfo = void*;

    Core::Service::Server&          server;
    std::map<SiteKey, SiteInfo>     siteMap;
    std::map<void*, int>            libCount;
    std::map<int, Binder>           portMap;
    int                             maxConnection;
    public:
        DynamicSiteLoader(Core::Service::Server& server);
        std::tuple<bool, int>      load(std::string const& site, int port, std::string const& host, std::string const& base);
        std::tuple<bool, int, int> unload(int port, std::string const& host, std::string const& base);
        void                       setMaxWaitingConnections(int max);
};

            }
        }
    }
}

#endif
