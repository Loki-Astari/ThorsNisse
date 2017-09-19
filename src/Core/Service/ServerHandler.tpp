#ifndef THORSANVIL_NISSE_NISSE_SERVER_HANDLER_TPP
#define THORSANVIL_NISSE_NISSE_SERVER_HANDLER_TPP

#include "ServerHandler.h"

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace Core
        {
            namespace Service
            {

template<typename Handler, typename Param>
inline ServerHandler<Handler, Param>::ServerHandler(Server& parent, Socket::ServerSocket&& socket, Param& param)
    : HandlerNonSuspendable(parent, std::move(socket), EV_READ)
    , param(param)
{}

template<typename Handler, typename Param>
inline ServerHandler<Handler, Param>::~ServerHandler()
{}

template<typename ActHand, typename Param>
inline short ServerHandler<ActHand, Param>::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    Socket::DataSocket accepted = stream.accept();
    addHandler<ActHand>(std::move(accepted), param);
    return EV_READ;
}

template<typename ActHand>
inline ServerHandler<ActHand, void>::ServerHandler(Server& parent, Socket::ServerSocket&& socket)
    : HandlerNonSuspendable(parent, std::move(socket), EV_READ)
{}

template<typename ActHand>
inline ServerHandler<ActHand, void>::~ServerHandler()
{}

template<typename ActHand>
inline short ServerHandler<ActHand, void>::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    Socket::DataSocket accepted = stream.accept();
    addHandler<ActHand>(std::move(accepted));
    return EV_READ;
}

            }
        }
    }
}

#endif
