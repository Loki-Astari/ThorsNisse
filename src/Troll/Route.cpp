#include "Route.h"

using namespace ThorsAnvil::Nisse::ProtocolHTTP;

Route::Route(std::string&& fr)
    : fullRoute(std::move(fr))
{
    std::size_t start = 0;
    for (auto find = fullRoute.find(":"); find != std::string::npos; find = fullRoute.find(":", start))
    {
        if (fullRoute[find - 1] != '/')
        {
            throw std::runtime_error("Invalid Route");
        }
        auto next = fullRoute.find('/', find);

        route.emplace_back(stx::string_view(fullRoute).substr(start, (find - start)));
        names.emplace_back(stx::string_view(fullRoute).substr(find + 1, (next - find - 1)));
        start = next;
    }
    route.emplace_back(stx::string_view(fullRoute).substr(start));
}

bool Route::operator<(Route const& rhs) const
{
    int test = 0;
    for (std::size_t loop = 0; loop < route.size() && loop < rhs.route.size(); ++loop)
    {
        test = route[loop].compare(rhs.route[loop]);
        if (test != 0)
        {
            return test < 0 ? true : false;
        }
    }
    test = route.size() - rhs.route.size();
    return test < 0 ? true : false;
}

bool Route::operator<(std::string const& rhs) const
{
    auto x = compare(rhs);
    return x < 0;
}

bool Route::operator<=(std::string const& rhs) const
{
    auto x = compare(rhs);
    return x <= 0;
}

bool Route::operator==(std::string const& rhs) const
{
    auto x = compare(rhs);
    return x == 0;
}

int Route::compare(std::string const& rhs) const
{
    std::size_t start = 0;
    for (std::size_t loop = 0; loop < route.size(); ++loop)
    {
        auto test = route[loop].compare(stx::string_view(rhs).substr(start, route[loop].size()));
        if (test != 0)
        {
            return test;
        }
        start = rhs.find('/', start + route[loop].size() + 1);
        start = start == std::string::npos ? rhs.size() : start;
    }
    return start - rhs.size();
}
