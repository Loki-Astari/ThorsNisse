#include "DynamicSiteLoader.h"
#include "HTTPProtocol.h"
#include "ThorsNisseCoreUtility/Utility.h"
#include <dlfcn.h>
#include <iostream>

using namespace ThorsAnvil::Nisse::Protocol::HTTP;


DynamicSiteLoader::DynamicSiteLoader(Core::Service::Server& server)
    : server(server)
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
        throw std::runtime_error(
            ThorsAnvil::Nisse::Core::Utility::buildErrorMessage(
                "ThorsAnvil::Nisse::Protocol::HTTP::DynamicSiteLoader::load: dlopen: Failed to load: ", site, " Error: ", eMessage));
    }

    using AddSiteFunction =  void (*)(ThorsAnvil::Nisse::Protocol::HTTP::Site& binder);
    using GetSiteAdder    =  AddSiteFunction (*)();

    GetSiteAdder    getSiteAdder = nullptr;
    *(void**) (&getSiteAdder) = ::dlsym(siteLib, "getSiteAdder");

    if (getSiteAdder == nullptr)
    {
        char const*  eMessage = ::dlerror();
        eMessage = (eMessage) ? eMessage : "Unknown";
        throw std::runtime_error(
            ThorsAnvil::Nisse::Core::Utility::buildErrorMessage(
                "ThorsAnvil::Nisse::Protocol::HTTP::DynamicSiteLoader::load: dlsym: Failed to load: ", site, " Error: ", eMessage));
    }

    AddSiteFunction addSite = (*getSiteAdder)();
    if (addSite == nullptr)
    {
        throw std::runtime_error(
            ThorsAnvil::Nisse::Core::Utility::buildErrorMessage(
                "ThorsAnvil::Nisse::Protocol::HTTP::DynamicSiteLoader::load: addSite: Failed to load: ", site, " Error: "));
    }

    bool portMapped = portMap.find(port) != portMap.end();

    Site            newSite;
    (*addSite)(newSite);

    siteMap[std::make_tuple(host, base, port)] = siteLib;
    Binder&     binder   = portMap[port];

    binder.addSite(host, base, std::move(newSite));

    if (!portMapped)
    {
        server.listenOn<ReadRequestHandler>(port, binder);
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
        throw std::domain_error(
            ThorsAnvil::Nisse::Core::Utility::buildErrorMessage(
                "ThorsAnvil::Nisse::Protocol::HTTP::DynamicSiteLoader::unload: Failed to unload: Could not find host:port/base: ", host, ":" , port, "/", base));
    }

    if (unload.second != 0)
    {
        std::cerr << this << ": " << "Disabled: " << host << ":" << port << "/" << base << ": Waiting for active calls to finish\n";
        return std::make_tuple(unload.first, unload.second, libCount[siteLib]);
    }

    --libCount[siteLib];
    siteMap.erase(find);
    if (libCount[siteLib] != 0)
    {
        std::cerr << this << ": " << "Disabled: " << host << ": " << port << "/" << base << ": Lib still bound\n";
        return std::make_tuple(true, 0, libCount[siteLib]);
    }

    int result = ::dlclose(siteLib);
    if (result != 0)
    {
        char const*  eMessage = ::dlerror();
        eMessage = (eMessage) ? eMessage : "Unknown";
        throw std::runtime_error(
            ThorsAnvil::Nisse::Core::Utility::buildErrorMessage(
                "ThorsAnvil::Nisse::Protocol::HTTP::DynamicSiteLoader::unload: dlclose: Failed to unload: host:port/base ", host, ":", port, "/", base, " Error: ", eMessage));
    }
    std::cerr << this << ": " << "UnLoaded: " << "----" << " " << host << ":" << port << "/" << base << "\n";
    return std::make_tuple(true, 0, 0);
}

#ifdef COVERAGE_TEST
/*
 * This code is only compiled into the unit tests for code coverage purposes
 * It is not part of the live code.
 */
#include "HTTPProtocol.h"
#include "ThorsNisseCoreService/Server.tpp"
#include "ThorsNisseCoreService/Handler.tpp"
template void ThorsAnvil::Nisse::Core::Service::Server::listenOn<ThorsAnvil::Nisse::Protocol::HTTP::ReadRequestHandler, ThorsAnvil::Nisse::Protocol::HTTP::Binder>(int, ThorsAnvil::Nisse::Protocol::HTTP::Binder&);
template ThorsAnvil::Nisse::Core::Service::ServerHandler<ThorsAnvil::Nisse::Protocol::HTTP::ReadRequestHandler, ThorsAnvil::Nisse::Protocol::HTTP::Binder>::ServerHandler(ThorsAnvil::Nisse::Core::Service::Server&, ThorsAnvil::Nisse::Core::Socket::ServerSocket&&, ThorsAnvil::Nisse::Protocol::HTTP::Binder&);
#endif
