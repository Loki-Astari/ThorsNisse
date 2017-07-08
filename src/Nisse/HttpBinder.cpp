#include "HttpBinder.h"

using namespace ThorsAnvil::Nisse;

void HTTPBinder::add(std::string const& path, HttpAction action)
{
    actionMap.emplace(path, action);
}

HttpAction& HTTPBinder::find(std::string const& path) const
{
    auto find = actionMap.find(path);
    if (find == actionMap.end())
    {
        // Need to perform a 404 Action if we don't hit anything.
        // TODO
    }
    return find->second;
}
