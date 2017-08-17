#include "Binder.h"
#include "ThorsNisseCoreSocket/Utility.h"
#include <dlfcn.h>
#include <dlfcn.h>


using namespace ThorsAnvil::Nisse::Protocol::HTTP;

void Site::add(Method method, std::string&& path, Action&& action)
{
    add(static_cast<int>(method), std::move(path), std::move(action));
}

void Site::add(int index, std::string&& path, Action&& action)
{
    actionMap[index].emplace(std::piecewise_construct,
                             std::forward_as_tuple(std::move(path)),
                             std::forward_as_tuple(std::move(action)));
}

std::pair<bool, Action&> Site::find(Method method, std::string const& path) const
{
    if (method == Method::Head)
    {
        method = Method::Get;
    }
    auto find = actionMap[static_cast<int>(method)].find(path);
    if (find == actionMap[static_cast<int>(method)].end())
    {
        find = actionMap[4].find(path);
        if (find == actionMap[4].end())
        {
            static Action noAction = [](Request&, Response&){};
            return {false, noAction};
        }
    }
    return {true, find->second};
}

Binder::Binder()
    : action404(getDefault404Action())
{}

void Binder::addSite(std::string const& host, Site&& site)
{
    siteMap.emplace(host, std::move(site));
}

Action& Binder::getDefault404Action()
{
    static Action   action404([](Request&, Response& response)
                                {
                                    response.resultCode                 = 404;
                                    response.resultMessage              = Resp_404;
                                    response.headers[Head_Date]         = getTimeString();
                                    response.headers[Head_Server]       = ServerName;
                                    response.headers[Head_ContentLen]   = "44";
                                    response.headers[Head_ContentType]  = "text/html";
                                    response.headers[Head_Connection]   = Connection_Closed;
                                    response.body << "<html><body><h1>Not Found</h1></body></html>";
                                }
                             );
    return action404;
}
void Binder::setCustome404Action(Action&& action)
{
    action404 = std::move(action);
}

Action const& Binder::find(Method method, std::string const& host, std::string const& path) const
{
    if (host != "")
    {
        auto findHost = siteMap.find(host);
        if (findHost != siteMap.end())
        {
            auto action = findHost->second.find(method, path);
            if (action.first)
            {
                return action.second;
            }
        }
    }
    auto blindHost = siteMap.find("");
    if (blindHost != siteMap.end())
    {
        auto action = blindHost->second.find(method, path);
        if (action.first)
        {
            return action.second;
        }
    }
    return action404;
}

void Binder::load(std::string const& site)
{
    void* siteLib = dlopen(site.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (siteLib == nullptr)
    {
        throw std::runtime_error(
            ThorsAnvil::Nisse::Core::Socket::buildErrorMessage(
                "ThorsAnvil::Nisse::ProtocolHTTP::Binder::load: dlopen: Failed to load: ", site, " Error: ", dlerror()));
    }
    /* Get rid of old error messages */
    dlerror();

    void (*addSite)(ThorsAnvil::Nisse::Protocol::HTTP::Binder& binder) = nullptr;

    *(void**) (&addSite) = dlsym(siteLib, "_Z7addSiteRN10ThorsAnvil5Nisse12ProtocolHTTP6BinderE");
    if (addSite == nullptr)
    {
        throw std::runtime_error(
            ThorsAnvil::Nisse::Core::Socket::buildErrorMessage(
                "ThorsAnvil::Nisse::ProtocolHTTP::Binder::load: dlsym: Failed to load: ", site, " Error: ", dlerror()));
    }

    (*addSite)(*this);
}