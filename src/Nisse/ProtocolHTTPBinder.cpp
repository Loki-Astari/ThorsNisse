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

void Binder::add(Method method, std::string const& path, Action action)
{
    actionMap[static_cast<int>(method)].emplace(path, action);
}

Action& Binder::find(Method method, std::string const& path) const
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

    auto find = actionMap[static_cast<int>(method)].find(path);
    if (find == actionMap[static_cast<int>(method)].end())
    {
        return action404;
    }
    return find->second;
}
