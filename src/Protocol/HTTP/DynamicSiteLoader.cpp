#include "DynamicSiteLoader.h"
#include "ThorsNisseCoreUtility/Utility.h"
#include <dlfcn.h>
#include <iostream>

using namespace ThorsAnvil::Nisse::Protocol::HTTP;


DynamicSiteLoader::DynamicSiteLoader(Core::Service::Server& server)
    : server(server)
{}

void DynamicSiteLoader::load(std::string const& site, int port, std::string const& host, std::string const& base)
{
    void* siteLib = dlopen(site.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (siteLib == nullptr)
    {
        throw std::runtime_error(
            ThorsAnvil::Nisse::Core::Utility::buildErrorMessage(
                "ThorsAnvil::Nisse::Protocol::HTTP::DynamicSiteLoader::load: dlopen: Failed to load: ", site, " Error: ", dlerror()));
    }

    using AddSiteFunction =  void (*)(ThorsAnvil::Nisse::Protocol::HTTP::BinderProxy& binder);
    using GetSiteAdder    =  AddSiteFunction (*)();

    GetSiteAdder    getSiteAdder = nullptr;
    *(void**) (&getSiteAdder) = dlsym(siteLib, "getSiteAdder");

    if (getSiteAdder == nullptr)
    {
        throw std::runtime_error(
            ThorsAnvil::Nisse::Core::Utility::buildErrorMessage(
                "ThorsAnvil::Nisse::Protocol::HTTP::DynamicSiteLoader::load: dlsym: Failed to load: ", site, " Error: ", dlerror()));
    }

    AddSiteFunction addSite = (*getSiteAdder)();
    if (addSite == nullptr)
    {
        throw std::runtime_error(
            ThorsAnvil::Nisse::Core::Utility::buildErrorMessage(
                "ThorsAnvil::Nisse::Protocol::HTTP::DynamicSiteLoader::load: addSite: Failed to load: ", site, " Error: "));
    }

    bool addListener = portMap.find(port) == portMap.end();

    Binder&         binder(portMap[port]);
    BinderProxy     proxy(binder, host, base);
    loadedLibs[{host, base}] = {siteLib, port};
    (*addSite)(proxy);

    if (addListener)
    {
        server.listenOn<ReadRequestHandler>(port, binder);
    }
    std::cerr << this << ": " << "Loaded: " << site << " " << host << ":" << port << "/" << base << "\n";
}

std::pair<bool, int> DynamicSiteLoader::unload(std::string const& host, std::string const& base)
{
    auto find = loadedLibs.find({host, base});
    if (find == loadedLibs.end())
    {
        throw std::runtime_error(
            ThorsAnvil::Nisse::Core::Utility::buildErrorMessage(
                "ThorsAnvil::Nisse::Protocol::HTTP::DynamicSiteLoader::unload: Failed to unload: Could not find host/base: ", host, "/", base));
    }

    SiteInfo const info = find->second;
    loadedLibs.erase(find);

    auto findBinder = portMap.find(std::get<1>(info));
    if (findBinder == portMap.end())
    {
        throw std::runtime_error(
            ThorsAnvil::Nisse::Core::Utility::buildErrorMessage(
                "ThorsAnvil::Nisse::Protocol::HTTP::DynamicSiteLoader::unload: Failed to unload: Could not find binder", std::get<1>(info), " For: ", host, "/", base));
    }

    Binder& binder = findBinder->second;
    auto unload = binder.remSite(host, base);

    if (!unload.first)
    {
        std::cerr << this << ": Unknown Site: " << host << ":" << std::get<1>(info) << "/" << base << "\n";
        return unload;
    }

    if (unload.second != 0)
    {
        std::cerr << this << ": " << "Disabled: " << "----" << " " << host << ":" << std::get<1>(info) << "/" << base << "\n";
        return unload;
    }

    int result = dlclose(std::get<0>(info));
    if (result != 0)
    {
        throw std::runtime_error(
            ThorsAnvil::Nisse::Core::Utility::buildErrorMessage(
                "ThorsAnvil::Nisse::Protocol::HTTP::DynamicSiteLoader::unload: dlclose: Failed to unload: host/port ", host, base, " From: ", std::get<1>(info), " Error: ", dlerror()));
    }
    std::cerr << this << ": " << "UnLoaded: " << "----" << " " << host << ":" << std::get<1>(info) << "/" << base << "\n";
    return unload;
}
