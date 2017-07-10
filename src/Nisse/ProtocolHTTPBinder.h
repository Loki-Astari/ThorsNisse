#ifndef THORSANVIL_NISSE_PROTOCOLHTTP_BINDER_H
#define THORSANVIL_NISSE_PROTOCOLHTTP_BINDER_H

#include "ProtocolHTTPTypes.h"

#include <map>
#include <array>
#include <string>
#include <functional>

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace ProtocolHTTP
        {

using Action = std::function<void(Request&, Response&)>;

class Binder
{
    public:

        void get(std::string const& path, Action action)    {add(Method::Get,    path, action);}
        void put(std::string const& path, Action action)    {add(Method::Put,    path, action);}
        void del(std::string const& path, Action action)    {add(Method::Delete, path, action);}
        void post(std::string const& path, Action action)   {add(Method::Post,   path, action);}
        Action& find(Method method, std::string const& path) const;
    private:
        void add(Method method, std::string const& path, Action action);
        mutable std::array<std::map<std::string, Action>, 4>   actionMap;
};

        }
    }
}

#endif
