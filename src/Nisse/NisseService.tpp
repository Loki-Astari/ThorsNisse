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
    handlers.emplace_back(std::make_unique<H>(*this, eventBase, std::forward<Args>(args)...));
}

    }
}

#ifndef COVERAGE_TEST
#include "NisseService.tpp"
#endif
#endif
