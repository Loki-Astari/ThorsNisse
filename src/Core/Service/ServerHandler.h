#ifndef THORSANVIL_NISSE_CORE_SERVICE_SERVER_HANDLER_H
#define THORSANVIL_NISSE_CORE_SERVICE_SERVER_HANDLER_H

#ifndef THORSANVIL_NISSE_CORE_SERVICE_SERVICE_H
#error "Please do not include >ThorsNisseCoreService/ServerHandler.h< directly. Include the >ThorsNisseCoreService/Service.h< file instead."
#endif

#include "ThorsSocket/Socket.h"

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace Core
        {
            namespace Service
            {
template<typename Stream> class HandlerNonSuspendable;

// @class
// An implementation of HandlerNonSuspendable that is used to accept connections and create other handlers.
template<typename ActHand, typename Param>
class ServerHandler: public HandlerNonSuspendable<ThorsSocket::ServerSocket>
{
    private:
        Param&                  param;
    public:
        ServerHandler(Server& parent, ThorsSocket::ServerSocket&& so, Param& param);
        ~ServerHandler();
        virtual short eventActivate(LibSocketId sockId, short eventType) override;
};

template<typename ActHand>
class ServerHandler<ActHand, void>: public HandlerNonSuspendable<ThorsSocket::ServerSocket>
{
    public:
        ServerHandler(Server& parent, ThorsSocket::ServerSocket&& so);
        ~ServerHandler();
        virtual short eventActivate(LibSocketId sockId, short eventType) override;
};

// @class
// An implementation of HandlerNonSuspendable that is used to implement the timer functionality.
class TimerHandler: public HandlerNonSuspendable<int>
{
    std::function<void()>        action;
    public:
        TimerHandler(Server& parent, double timeOut, std::function<void()>&& action);
        virtual short eventActivate(LibSocketId sockId, short eventType) override;
};

            }
        }
    }
}

#endif
