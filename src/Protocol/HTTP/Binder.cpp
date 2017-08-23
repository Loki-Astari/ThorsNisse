#include "Binder.h"
#include "ThorsNisseCoreUtility/Utility.h"


using namespace ThorsAnvil::Nisse::Protocol::HTTP;

Site::Site()
    : activeItems(0)
{}

Site::Site(Site&& other) noexcept
    : activeItems(0)
{
    swap(other);
}

Site& Site::operator=(Site&& other) noexcept
{
    swap(other);
    return *this;
}

void Site::swap(Site& other) noexcept
{
    using std::swap;
    swap(actionMap,   other.actionMap);
    swap(activeItems, other.activeItems);
}

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

struct IncDecRaii
{
    int&    value;
    IncDecRaii(int& value): value(value)    {++value;}
    ~IncDecRaii()                           {--value;}
};

std::pair<bool, Action> Site::find(Method method, std::string const& path) const
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
            return {false, [](Request&, Response&){}};
        }
    }
    return {true, [action = find->second, &value = this->activeItems](Request& request, Response& response){IncDecRaii count(value);action(request, response);}};
}

Binder::Binder()
    : action404(getDefault404Action())
{}

void Binder::addSite(std::string const& host, std::string const& base, Site&& site)
{
    (void)base;
    siteMap[host]   = std::move(site);
}

std::pair<bool, int> Binder::remSite(std::string const& host, std::string const& base)
{
    (void)base;
    if (siteMap.find(host) != siteMap.end())
    {
        Site    unload = std::move(siteMap[host]);
        siteMap[host].activeItems = unload.activeItems;
        return {true, unload.activeItems};
    }
    return {false, 0};
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

Action Binder::find(Method method, std::string const& host, std::string const& path) const
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
