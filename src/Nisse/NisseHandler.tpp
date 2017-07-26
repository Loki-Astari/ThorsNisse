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

template<typename Handler, typename Param>
inline ServerHandler<Handler, Param>::ServerHandler(NisseService& parent, ThorsAnvil::Socket::ServerSocket&& so, Param& param)
    : NisseHandler(parent, so.getSocketId(), EV_READ)
    , socket(std::move(so))
    , param(param)
{}

template<typename Handler, typename Param>
inline ServerHandler<Handler, Param>::~ServerHandler()
{}

template<typename Handler, typename Param>
inline void ServerHandler<Handler, Param>::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    ThorsAnvil::Socket::DataSocket accepted = socket.accept();
    addHandler<Handler>(std::move(accepted), param);
}

template<typename Handler>
inline ServerHandler<Handler, void>::ServerHandler(NisseService& parent, ThorsAnvil::Socket::ServerSocket&& so)
    : NisseHandler(parent, so.getSocketId(), EV_READ)
    , socket(std::move(so))
{}

template<typename Handler>
inline ServerHandler<Handler, void>::~ServerHandler()
{}

template<typename Handler>
inline void ServerHandler<Handler, void>::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    ThorsAnvil::Socket::DataSocket accepted = socket.accept();
    addHandler<Handler>(std::move(accepted));
}

    }
}

#endif
