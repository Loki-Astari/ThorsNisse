#ifndef THORSANVIL_NISSE_NISSE_SERVICE_TPP
#define THORSANVIL_NISSE_NISSE_SERVICE_TPP

#include "NisseHandler.h"
#include "ThorsNisseSocket/Socket.h"
#include <memory>
#include <vector>

namespace ThorsAnvil
{
    namespace Nisse
    {

template<typename Handler>
inline void NisseService::listenOn(int port)
{
    addHandler<ServerHandler<Handler, void>>(ThorsAnvil::Socket::ServerSocket(port));
}

template<typename Handler, typename Param>
inline void NisseService::listenOn(int port, Param& param)
{
    addHandler<ServerHandler<Handler, Param>>(ThorsAnvil::Socket::ServerSocket(port), param);
}

template<typename Func>
void NisseService::addTimer(double timeOut, Func&& action)
{
    addHandler<TimerHandler>(timeOut, std::move(action));
}

template<typename H, typename... Args>
inline void NisseService::addHandler(Args&&... args)
{
    NisseManagHandler   value = std::make_unique<H>(*this, std::forward<Args>(args)...);
    NisseHandler*       key   = value.get();
    handlers.emplace(key, std::move(value));
}

    }
}

#ifndef COVERAGE_TEST
#include "NisseService.tpp"
#endif
#endif
