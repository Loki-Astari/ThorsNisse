#ifndef THORSANVIL_NISSE_HTTP_BINDER_H
#define THORSANVIL_NISSE_HTTP_BINDER_H

#include "HttpTypes.h"

#include <map>
#include <string>
#include <functional>

namespace ThorsAnvil
{
    namespace Nisse
    {

using HttpAction = std::function<void(HTTPRequest&, HTTPResponse&)>;

class HTTPBinder
{
    public:

        void add(std::string const& path, HttpAction action);
        HttpAction& find(std::string const& path) const;
    private:
        mutable std::map<std::string, HttpAction>   actionMap;
};

    }
}

#endif
