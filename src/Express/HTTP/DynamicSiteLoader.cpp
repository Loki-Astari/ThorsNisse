#include "DynamicSiteLoader.h"
#include "HTTPProtocol.h"
#include "ThorsLogging/ThorsLogging.h"
#include "ThorsIOUtil/Utility.h"
#include "ThorsSocket/Socket.h"
#include <dlfcn.h>
#include <iostream>

using namespace ThorsAnvil::Nisse::Protocol::HTTP;


DynamicSiteLoader::DynamicSiteLoader(Core::Service::Server& server)
    : server(server)
    , maxConnection(ThorsAnvil::ThorsSocket::ServerSocket::maxConnectionBacklog)
{}

std::tuple<bool, int> DynamicSiteLoader::load(std::string const& site, int port, std::string const& host, std::string const& base)
{
    if (siteMap.find(std::make_tuple(host, base, port)) != siteMap.end())
    {
        return std::make_tuple(false, 0);
    }
    void* siteLib = ::dlopen(site.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (siteLib == nullptr)
    {
        char const*  eMessage = ::dlerror();
        eMessage = (eMessage) ? eMessage : "Unknown";
        ThorsLogAndThrow("ThorsAnvil::Nisse::Protocol::HTTP::DynamicSiteLoader::", "load", " dlopen: Failed to load: ", site.c_str(), " Error: ", eMessage);
    }

    using AddSiteFunction =  void (*)(ThorsAnvil::Nisse::Protocol::HTTP::Site& binder);
    using GetSiteAdder    =  AddSiteFunction (*)();

    GetSiteAdder    getSiteAdder = nullptr;
    *(void**) (&getSiteAdder) = ::dlsym(siteLib, "getSiteAdder");

    if (getSiteAdder == nullptr)
    {
        char const*  eMessage = ::dlerror();
        eMessage = (eMessage) ? eMessage : "Unknown";
        ThorsLogAndThrow("ThorsAnvil::Nisse::Protocol::HTTP::DynamicSiteLoader::", "load", " dlsym: Failed to load: ", site.c_str(), " Error: ", eMessage);
    }

    AddSiteFunction addSite = (*getSiteAdder)();
    if (addSite == nullptr)
    {
        ThorsLogAndThrow("ThorsAnvil::Nisse::Protocol::HTTP::DynamicSiteLoader::", "load", " addSite: Failed to load: ", site.c_str(), " Error: ");
    }

    bool portMapped = portMap.find(port) != portMap.end();

    Site            newSite;
    (*addSite)(newSite);

    siteMap[std::make_tuple(host, base, port)] = siteLib;
    Binder&     binder   = portMap[port];

    binder.addSite(host, base, std::move(newSite));

    if (!portMapped)
    {
        using ThorsAnvil::Nisse::Core::Service::ServerConnection;
        server.listenOn<ReadRequestHandler>(ServerConnection(port, maxConnection), binder);
    }
    ++libCount[siteLib];
    std::cerr << this << ": " << "Loaded: " << site << " " << host << ":" << port << "/" << base << "\n";
    return std::make_tuple(true, libCount[siteLib]);
}

std::tuple<bool, int, int> DynamicSiteLoader::unload(int port, std::string const& host, std::string const& base)
{
    auto find = siteMap.find(std::make_tuple(host, base, port));
    if (find == siteMap.end())
    {
        std::cerr << this << ": Unknown Site: " << host << ":" << port << "/" << base << "\n";
        return std::make_tuple(false, 0, 0);
    }

    SiteInfo    siteLib = find->second;
    Binder&     binder  = portMap[port];
    auto        unload  = binder.remSite(host, base);

    if (!unload.first)
    {
        ThorsLogAndThrowAction(2, std::domain_error, "ThorsAnvil::Nisse::Protocol::HTTP::DynamicSiteLoader::", "unload", " Failed to unload: Could not find host:port/base: ", host.c_str(), ":" , port, "/", base);
    }

    if (unload.second != 0)
    {
        ThorsMessage(WARNING, "ThorsAnvil::Nisse::Protocol::HTTP::DynamicSiteLoader::", "unload", this, ": Disabled: ", host, ":", port, "/", base, ": Waiting for active calls to finish");
        return std::make_tuple(unload.first, unload.second, libCount[siteLib]);
    }

    --libCount[siteLib];
    siteMap.erase(find);
    if (libCount[siteLib] != 0)
    {
        ThorsMessage(WARNING, "ThorsAnvil::Nisse::Protocol::HTTP::DynamicSiteLoader::", "unload", this, ": Disabled: ", host, ": ", port, "/", base, ": Lib still bound");
        return std::make_tuple(true, 0, libCount[siteLib]);
    }

    int result = ::dlclose(siteLib);
    if (result != 0)
    {
        char const*  eMessage = ::dlerror();
        eMessage = (eMessage) ? eMessage : "Unknown";
        ThorsLogAndThrow("ThorsAnvil::Nisse::Protocol::HTTP::DynamicSiteLoader::", "unload", " dlclose: Failed to unload: host:port/base ", host.c_str(), ":", port, "/", base, " Error: ", eMessage);
    }
    ThorsMessage(WARNING, "ThorsAnvil::Nisse::Protocol::HTTP::DynamicSiteLoader::", "unload", this, ": UnLoaded: ", "---- ", host, ":", port, "/", base);
    return std::make_tuple(true, 0, 0);
}

void DynamicSiteLoader::setMaxWaitingConnections(int max)
{
    maxConnection = max;
}
