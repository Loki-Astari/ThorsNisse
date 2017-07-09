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

void Binder::add(std::string const& path, Action action)
{
    actionMap.emplace(path, action);
}

Action& Binder::find(std::string const& path) const
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

    auto find = actionMap.find(path);
    if (find == actionMap.end())
    {
        return action404;
    }
    return find->second;
}
