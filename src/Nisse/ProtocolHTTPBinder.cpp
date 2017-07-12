#include "ProtocolHTTPBinder.h"

using namespace ThorsAnvil::Nisse::ProtocolHTTP;

static char const* getTimeString()
{
    using TimeT = std::time_t;
    using TimeI = std::tm;

    TimeT   rawtime;
    TimeI*  timeinfo;

    ::time(&rawtime);
    timeinfo = ::localtime(&rawtime);

    return ::asctime(timeinfo);
}

void Site::add(Method method, std::string const& path, Action action)
{
    actionMap[static_cast<int>(method)].emplace(path, action);
}

std::pair<bool, Action&> Site::find(Method method, std::string const& path) const
{
    auto find = actionMap[static_cast<int>(method)].find(path);
    if (find == actionMap[static_cast<int>(method)].end())
    {
        static Action noAction;
        return {false, noAction};
    }
    return {true, find->second};
}

void Binder::addSite(std::string const& host, Site&& site)
{
    siteMap.emplace(host, std::move(site));
}

Action& Binder::find(Method method, std::string const& host, std::string const& path) const
{
    static Action   action404([](Request&, Response& response)
                                {
                                    response.resultCode                 = 404;
                                    response.resultMessage              = "Not Found";
                                    response.headers["Date"]            = getTimeString();
                                    response.headers["Server"]          = "Nisse";
                                    response.headers["Content-Length"]  = "44";
                                    response.headers["Content-Type"]    = "text/html";
                                    response.headers["Connection"]      = "Closed";
                                    response.body << "<html><body><h1>Not Found</h1></body></html>";
                                }
                             );

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
