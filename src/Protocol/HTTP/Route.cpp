#include "Route.h"

using namespace ThorsAnvil::Nisse::Protocol::HTTP;

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
        // Check the current segment against the route.
        auto test = route[loop].compare(stx::string_view(rhs).substr(start, route[loop].size()));
        if (test != 0)
        {
            // They are not equal so can quit
            return test;
        }

        // The rhs matched equal to part of the route we just checked.
        // So move the start position along that amount we matched.
        std::size_t newStart = start + route[loop].size();
        if (newStart == rhs.size())
        {
            // There is no more rhs.
            // So if there is no names to match they are equal.
            //    Note: If there is no more names then there is no more route
            // If there is a name section to match then the rhs is smaller as it does not have a name
            return names.size() <= loop ? 0 : -1;
        }
        else
        {
            // There is more rhs to match.
            if (rhs[newStart - 1] != '/')
            {
                // If the last character is not a '/' then we have a route segments that is longer
                // rhs that is longer. So we return +1
                return +1;
            }
            // The path matched an exact route segment. So the next part is a name.
            // We ignore the name on the rhs (as any text matches a name).
            start = rhs.find('/', newStart + 1);
            start = start == std::string::npos ? rhs.size() : start;
        }
    }
    return start - rhs.size();
}
