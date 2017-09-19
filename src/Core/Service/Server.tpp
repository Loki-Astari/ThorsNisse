#ifndef THORSANVIL_NISSE_NISSE_SERVICE_TPP
#define THORSANVIL_NISSE_NISSE_SERVICE_TPP

#include "ServerHandler.h"
#include "ThorsNisseCoreSocket/Socket.h"
#include <memory>
#include <vector>

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace Core
        {
            namespace Service
            {

template<typename Handler>
inline void Server::listenOn(ServerConnection const& info)
{
    addHandler<ServerHandler<Handler, void>>(Socket::ServerSocket(info.port, false, info.maxConnections));
}

template<typename Handler, typename Param>
inline void Server::listenOn(ServerConnection const& info, Param& param)
{
    addHandler<ServerHandler<Handler, Param>>(Socket::ServerSocket(info.port, false, info.maxConnections), param);
}

template<typename H, typename... Args>
inline HandlerBase& Server::addHandler(Args&&... args)
{
    NisseManagHandler   value = std::make_unique<H>(*this, std::forward<Args>(args)...);
    HandlerBase*        key   = value.get();
    handlers.emplace(key, std::move(value));
    return *key;
}

template<typename H, typename... Args>
inline void Server::transferHandler(Args&&... args)
{
    if (currentHandler == nullptr || !currentHandler->suspendable())
    {
        throw std::runtime_error("Can not transfer handlers when not running a current handler (or the current one is non-suspendable)");
    }
    HandlerBase& handler = addHandler<H>(std::forward<Args>(args)...);
    handler.setSuspend(*currentHandler);
}

            }
        }
    }
}

#endif
