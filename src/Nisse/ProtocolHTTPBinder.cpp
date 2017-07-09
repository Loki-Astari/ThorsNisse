#include "ProtocolHTTPBinder.h"

using namespace ThorsAnvil::Nisse::ProtocolHTTP;

class TimePrinter
{
    private:
        char const* time;
    public:
        TimePrinter()
            : time(getTimeString())
        {}
        friend std::ostream& operator<<(std::ostream& str, TimePrinter const& data)
        {
            str.write(data.time, 24);
            return str;
        }
    private:
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
};


void Binder::add(std::string const& path, Action action)
{
    actionMap.emplace(path, action);
}

Action& Binder::find(std::string const& path) const
{
    static Action   action404([](Request&, Response& response)
                                {
                                    response.body << "HTTP/1.1 404 Not Found\r\n"
                                                  << "Date: " << TimePrinter() << "\r\n"
                                                  << "Server: Nisse\r\n"
                                                  << "Content-Length: 44\r\n"
                                                  << "Content-Type: text/html\r\n"
                                                  << "Connection: Closed\r\n"
                                                  << "\r\n"
                                                  << "<html><body><h1>Not Found</h1></body></html>";
                                }
                             );

    auto find = actionMap.find(path);
    if (find == actionMap.end())
    {
        return action404;
    }
    return find->second;
}
