#include "DynamicSiteLoader.h"
#include "HTTPProtocol.h"
#include "ThorsNisseCoreUtility/Utility.h"
#include <dlfcn.h>

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
}

void DynamicSiteLoader::unload(std::string const& host, std::string const& base)
{
    auto find = loadedLibs.find({host, base});
    if (find == loadedLibs.end())
    {
        throw std::runtime_error(
            ThorsAnvil::Nisse::Core::Utility::buildErrorMessage(
                "ThorsAnvil::Nisse::Protocol::HTTP::DynamicSiteLoader::unload: Failed to unload: Could not find host/base", host, " ", base));
    }

    SiteInfo const& info = find->second;
    auto findBinder = portMap.find(std::get<1>(info));
    if (findBinder == portMap.end())
    {
        throw std::runtime_error(
            ThorsAnvil::Nisse::Core::Utility::buildErrorMessage(
                "ThorsAnvil::Nisse::Protocol::HTTP::DynamicSiteLoader::unload: Failed to unload: Could not find binder", std::get<1>(info), " For: ", host, " ", base));
    }

    Binder& binder = findBinder->second;
    binder.remSite(host, base);

    int result = dlclose(std::get<0>(info));
    if (result != 0)
    {
        throw std::runtime_error(
            ThorsAnvil::Nisse::Core::Utility::buildErrorMessage(
                "ThorsAnvil::Nisse::Protocol::HTTP::DynamicSiteLoader::unload: dlclose: Failed to unload: host/port ", host, base, " From: ", std::get<1>(info), " Error: ", dlerror()));
    }
}
