#ifndef THORSANVIL_NISSE_NISSE_SERVICE_TPP
#define THORSANVIL_NISSE_NISSE_SERVICE_TPP

#include "NisseHandler.h"
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
inline void NisseService::listenOn(int port)
{
    addHandler<ServerHandler<Handler, void>>(Socket::ServerSocket(port));
}

template<typename Handler, typename Param>
inline void NisseService::listenOn(int port, Param& param)
{
    addHandler<ServerHandler<Handler, Param>>(Socket::ServerSocket(port), param);
}

template<typename H, typename... Args>
inline NisseHandler& NisseService::addHandler(Args&&... args)
{
    NisseManagHandler   value = std::make_unique<H>(*this, std::forward<Args>(args)...);
    NisseHandler*       key   = value.get();
    handlers.emplace(key, std::move(value));
    return *key;
}

template<typename H, typename... Args>
inline void NisseService::transferHandler(Args&&... args)
{
    if (currentHandler == nullptr)
    {
        throw std::runtime_error("Can not transfer handlers when not running a current handler");
    }
    NisseHandler& handler = addHandler<H>(std::forward<Args>(args)...);
    handler.setSuspend(*currentHandler);
}

            }
        }
    }
}

#ifndef COVERAGE_TEST
#include "NisseService.tpp"
#endif
#endif
