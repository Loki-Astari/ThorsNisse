#ifndef THORSANVIL_NISSE_NISSE_HANDLER_TPP
#define THORSANVIL_NISSE_NISSE_HANDLER_TPP

#include "Server.h"
#include <iostream>

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace Core
        {
            namespace Service
            {

template<typename H, typename... Args>
inline void Handler::addHandler(Args&&... args)
{
    parent.addHandler<H>(std::forward<Args>(args)...);
}

template<typename H, typename... Args>
inline void Handler::moveHandler(Args&&... args)
{
    dropHandler();
    parent.addHandler<H>(std::forward<Args>(args)...);
}

template<typename Handler, typename Param>
inline ServerHandler<Handler, Param>::ServerHandler(Server& parent, Socket::ServerSocket&& so, Param& param)
    : Handler(parent, so.getSocketId(), EV_READ)
    , socket(std::move(so))
    , param(param)
{}

template<typename Handler, typename Param>
inline ServerHandler<Handler, Param>::~ServerHandler()
{}

template<typename ActHand, typename Param>
inline short ServerHandler<ActHand, Param>::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    Socket::DataSocket accepted = socket.accept();
    addHandler<ActHand>(std::move(accepted), param);
    return EV_READ;
}

template<typename ActHand>
inline ServerHandler<ActHand, void>::ServerHandler(Server& parent, Socket::ServerSocket&& so)
    : Handler(parent, so.getSocketId(), EV_READ)
    , socket(std::move(so))
{}

template<typename ActHand>
inline ServerHandler<ActHand, void>::~ServerHandler()
{}

template<typename ActHand>
inline short ServerHandler<ActHand, void>::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    Socket::DataSocket accepted = socket.accept();
    addHandler<ActHand>(std::move(accepted));
    return EV_READ;
}

            }
        }
    }
}

#endif
