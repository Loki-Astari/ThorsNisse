#ifndef THORSANVIL_NISSE_PROTOCOLHTTP_BINDER_H
#define THORSANVIL_NISSE_PROTOCOLHTTP_BINDER_H

#include "ProtocolHTTPTypes.h"

#include <map>
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

        void add(std::string const& path, Action action);
        Action& find(std::string const& path) const;
    private:
        mutable std::map<std::string, Action>   actionMap;
};

        }
    }
}

#endif
