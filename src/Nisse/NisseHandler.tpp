#ifndef THORSANVIL_NISSE_NISSE_HANDLER_TPP
#define THORSANVIL_NISSE_NISSE_HANDLER_TPP

#include "NisseService.h"

namespace ThorsAnvil
{
    namespace Nisse
    {

template<typename H, typename... Args>
inline void NisseHandler::addHandler(Args&&... args)
{
    parent.addHandler<H>(std::forward<Args>(args)...);
}

template<typename H, typename... Args>
inline void NisseHandler::moveHandler(Args&&... args)
{
    dropHandler();
    parent.addHandler<H>(std::forward<Args>(args)...);
}

template<typename Handler>
inline ServerHandler<Handler>::ServerHandler(NisseService& parent, LibEventBase* base, ThorsAnvil::Socket::ServerSocket&& so)
    : NisseHandler(parent, base, so.getSocketId(), EV_READ)
    , socket(std::move(so))
{}

template<typename Handler>
inline void ServerHandler<Handler>::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    ThorsAnvil::Socket::DataSocket accepted = socket.accept();
    addHandler<Handler>(std::move(accepted));
}

    }
}

#endif
