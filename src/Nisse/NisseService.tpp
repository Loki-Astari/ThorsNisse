#ifndef THORSANVIL_NISSE_NISSE_SERVICE_TPP
#define THORSANVIL_NISSE_NISSE_SERVICE_TPP

#include "NisseHandler.h"
#include "ThorsSocket/Socket.h"
#include <memory>
#include <vector>

namespace ThorsAnvil
{
    namespace Nisse
    {

template<typename Handler>
inline void NisseService::listenOn(int port)
{
    addHandler<ServerHandler<Handler>>(ThorsAnvil::Socket::ServerSocket(port));
}

template<typename H, typename... Args>
inline void NisseService::addHandler(Args&&... args)
{
    NisseManagHandler   value = std::make_unique<H>(*this, eventBase.get(), std::forward<Args>(args)...);
    NisseHandler*       key   = value.get();
    ((void)key);
    handlers.emplace(key, std::move(value));
}

    }
}

#ifndef COVERAGE_TEST
#include "NisseService.tpp"
#endif
#endif
