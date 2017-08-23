#ifndef THORSANVIL_NISSE_PROTOCOL_HTTP_BINDER_H
#define THORSANVIL_NISSE_PROTOCOL_HTTP_BINDER_H

#include "Types.h"
#include "Route.h"

#include <map>
#include <array>
#include <utility>
#include <string>
#include <functional>

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace Protocol
        {
            namespace HTTP
            {

using Action = std::function<void(Request&, Response&)>;

/*
 * A site represents the paths on a particular domain.
 * So you have a site object for each domain that your server is hosting.
 *
 * You then bind the site object to a domain using the Binder.
 * You can bind multiple domains in the binder. Then you pass the binder as
 * a parameter to the `listenOn()` method of the NisseServer. When a request is
 * received and `ReadRequestHandler` object is created it is passed the binder
 * object as a parameter.
 *
 *      Site        thorsAnvil;
 *      thorsAnvil.get("/user/", [](Request& request, Response& response) {
 *                                          response.responseCode            = 200;
 *                                          response.message                 = "OK";
 *                                          response.header["Content-Type:"] = "text/json";
 *                                          response.body << "{user: " << getUserId << "}"
 *                    );
 *      Binder      host;
 *      host.addSite("ThorsAnvil.com", thorsAnvil);
 *
 *      NisseServer server;
 *      server.listenOn<ReadRequestHandler>(80, host);
 */
class Site
{
    public:
        Site();
        Site(Site&&) noexcept;
        Site& operator=(Site&&) noexcept;
        void swap(Site&) noexcept;

        void get(std::string&& path, Action&& action)    {add(Method::Get,    std::move(path), std::move(action));}
        void put(std::string&& path, Action&& action)    {add(Method::Put,    std::move(path), std::move(action));}
        void del(std::string&& path, Action&& action)    {add(Method::Delete, std::move(path), std::move(action));}
        void post(std::string&& path, Action&& action)   {add(Method::Post,   std::move(path), std::move(action));}
        void all(std::string&& path, Action&& action)    {add(4,              std::move(path), std::move(action));}
        std::pair<bool, Action> find(Method method, std::string const& path) const;
    private:
        void add(Method method, std::string&& path, Action&& action);
        void add(int index, std::string&& path, Action&& action);

        mutable std::array<std::map<Route, Action, RouteTester>, 5>   actionMap;

        friend class Binder;
        mutable int activeItems;
};

class Binder
{
    static Action& getDefault404Action();
    private:
        Action                      action404;
        std::map<std::string, Site> siteMap;
    public:
        Binder();
        void setCustome404Action(Action&& action);
        void addSite(std::string const& host, std::string const& base, Site&& site);
        std::pair<bool, int> remSite(std::string const& host, std::string const& base);

        Action find(Method method, std::string const& host, std::string const& path) const;
};

class BinderProxy
{
    Binder&         binder;
    std::string     host;
    std::string     base;
    public:
        BinderProxy(Binder& binder, std::string const& host, std::string const& base)
            : binder(binder)
            , host(host)
            , base(base)
        {}
        void addSite(Site&& site)
        {
            binder.addSite(host, base, std::move(site));
        }
};

            }
        }
    }
}

#endif
