#include "ProtocolHTTPBinder.h"

using namespace ThorsAnvil::Nisse::ProtocolHTTP;

void Binder::add(std::string const& path, Action action)
{
    actionMap.emplace(path, action);
}

Action& Binder::find(std::string const& path) const
{
    auto find = actionMap.find(path);
    if (find == actionMap.end())
    {
        // Need to perform a 404 Action if we don't hit anything.
        // TODO
    }
    return find->second;
}
