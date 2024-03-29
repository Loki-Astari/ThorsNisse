#ifndef THORSANVIL_NISSE_CORE_SERVICE_SERVER_HANDLER_TPP
#define THORSANVIL_NISSE_CORE_SERVICE_SERVER_HANDLER_TPP

#ifndef THORSANVIL_NISSE_CORE_SERVICE_SERVICE_H
#error "Please do not include >ThorsNisseCoreService/ServerHandler.tpp< directly. Include the >ThorsNisseCoreService/Service.h< file instead."
#endif

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace Core
        {
            namespace Service
            {

template<typename Handler, typename Param>
inline ServerHandler<Handler, Param>::ServerHandler(Server& parent, ThorsSocket::ServerSocket&& socket, Param& param)
    : HandlerNonSuspendable(parent, std::move(socket), EV_READ)
    , param(param)
{}

template<typename Handler, typename Param>
inline ServerHandler<Handler, Param>::~ServerHandler()
{}

template<typename ActHand, typename Param>
inline short ServerHandler<ActHand, Param>::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    ThorsSocket::DataSocket accepted = stream.accept([](int fd){return std::make_unique<ThorsAnvil::ThorsSocket::ConnectionNormal>(fd);});
    addHandler<ActHand>(std::move(accepted), param);
    return EV_READ;
}

template<typename ActHand>
inline ServerHandler<ActHand, void>::ServerHandler(Server& parent, ThorsSocket::ServerSocket&& socket)
    : HandlerNonSuspendable(parent, std::move(socket), EV_READ)
{}

template<typename ActHand>
inline ServerHandler<ActHand, void>::~ServerHandler()
{}

template<typename ActHand>
inline short ServerHandler<ActHand, void>::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    ThorsSocket::DataSocket accepted = stream.accept([](int fd){return std::make_unique<ThorsAnvil::ThorsSocket::ConnectionNormal>(fd);});
    addHandler<ActHand>(std::move(accepted));
    return EV_READ;
}

            }
        }
    }
}

#endif
