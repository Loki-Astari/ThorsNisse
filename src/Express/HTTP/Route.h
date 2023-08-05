#ifndef THORSANVIL_NISSE_PROTOCOL_HTTP_ROUTE_H
#define THORSANVIL_NISSE_PROTOCOL_HTTP_ROUTE_H

#include <string>
#include <vector>
#include <ostream>
#include <string_view>

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace Protocol
        {
            namespace HTTP
            {

// @class
class Route
{
    /*
     * The route.size() should equal or be one greater than names.size()
     *
     * Each element in route represents a fixed-sub path of the route.
     *      Each element is non empty and starts with '/' and ends with '/' (Except last value)
     *      Note: the shortest element is: '/'  (starts and ends with the same character)
     *      Note: the last element in path may be a resource (if route.size() is one larger than names.size())
     *            and thus not need to end in '/'
     *
     * Each element in names represents a single segment of the route.
     *      This segment can contain any value and will be passed to the request
     *      object in the `param` field (a key/value map)
     *      The elements in this vector represent the keys the values are obtained from
     *      in input route.
     *
     *  Example:
     *      Created with:   /person/:id/value/action/:type/go.json
     *      Will generate:
     *              route: {"/person/", "/value/action/", "/go.json"}
     *              names: {"id", "type"}
     *
     *      This will match against:
     *          route:  /person/12345/value/action/activate/go.json
     *          param:  {"id": "12345", "type": "activate"}
     *
     * Two routes are equal if all the elements in `route` match.
     * The param obejct will be available via the request object.
     */
    const std::string                fullRoute;
    std::vector<std::string_view>    route;
    std::vector<std::string_view>    names;

    public:
        Route(std::string&& fullRoute);
        Route(Route const&)            = delete;
        Route& operator=(Route const&) = delete;
        Route(Route&&) = default;

        bool operator<(Route const& rhs) const;
        bool operator<(std::string const& rhs) const;
        bool operator<=(std::string const& rhs) const;
        bool operator==(std::string const& rhs) const;

    private:
        int compare(std::string const& rhs) const;

        friend std::ostream& operator<<(std::ostream& s, Route const& r)
        {
            return s << r.fullRoute;
        }
};

// @class
struct RouteTester
{
    using is_transparent = std::true_type;

    bool operator()(std::string const& lhs, Route const& rhs) const {return !(rhs <= lhs);}
    bool operator()(Route const& lhs, std::string const& rhs) const {return lhs < rhs;}
    bool operator()(Route const& lhs, Route const& rhs)       const {return lhs < rhs;}
};

            }
        }
    }
}

#endif
