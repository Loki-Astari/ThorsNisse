#ifndef THORSANVIL_NISSE_TEST_ACTION_H
#define THORSANVIL_NISSE_TEST_ACTION_H

#include "../Server.h"
#include "../Handler.h"
#include "../CoRoutine.h"
#include "ThorsNisseCoreSocket/Socket.h"

class Action: public ThorsAnvil::Nisse::Core::Service::HandlerNonSuspendable
{
    ThorsAnvil::Nisse::Core::Service::Server&           service;
    ThorsAnvil::Nisse::Core::Socket::DataSocket         dataSocket;
    public:
        Action(ThorsAnvil::Nisse::Core::Service::Server& parent, ThorsAnvil::Nisse::Core::Socket::DataSocket&& ds)
            : HandlerNonSuspendable(parent, ds.getSocketId(), EV_READ | EV_PERSIST)
            , service(parent)
            , dataSocket(std::move(ds))
        {}
        virtual short eventActivate(ThorsAnvil::Nisse::Core::Service::LibSocketId /*sockId*/, short /*eventType*/) override
        {
            service.flagShutDown();
            return 0;
        }
};
class ActionUnReg: public ThorsAnvil::Nisse::Core::Service::HandlerNonSuspendable
{
    ThorsAnvil::Nisse::Core::Service::Server&           service;
    ThorsAnvil::Nisse::Core::Socket::DataSocket         dataSocket;
    public:
        ActionUnReg(ThorsAnvil::Nisse::Core::Service::Server& parent, ThorsAnvil::Nisse::Core::Socket::DataSocket&& ds)
            : HandlerNonSuspendable(parent, ds.getSocketId(), EV_READ | EV_PERSIST)
            , service(parent)
            , dataSocket(std::move(ds))
        {}
        virtual short eventActivate(ThorsAnvil::Nisse::Core::Service::LibSocketId /*sockId*/, short /*eventType*/) override
        {
            service.flagShutDown();
            dropHandler();
            return 0;
        }
};
class SwapHandler: public ThorsAnvil::Nisse::Core::Service::HandlerNonSuspendable
{
    std::tuple<bool, bool, bool>& hit;
    public:
        SwapHandler(ThorsAnvil::Nisse::Core::Service::Server& server, std::tuple<bool, bool, bool>& hit, short socketId)
            : HandlerNonSuspendable(server, socketId, EV_READ)
            , hit(hit)
        {}
        virtual short eventActivate(ThorsAnvil::Nisse::Core::Service::LibSocketId, short)
        {
            std::get<2>(hit) = true;
            dropHandler();
            return 0;
        }
};

class TestHandler: public ThorsAnvil::Nisse::Core::Service::HandlerSuspendable
{
    ThorsAnvil::Nisse::Core::Service::Server&   server;
    ThorsAnvil::Nisse::Core::Socket::DataSocket socket;
    std::tuple<bool, bool, bool>&               hit;

    public:
        TestHandler(ThorsAnvil::Nisse::Core::Service::Server& server, ThorsAnvil::Nisse::Core::Socket::DataSocket&& so, std::tuple<bool, bool, bool>& hit)
            : HandlerSuspendable(server, so.getSocketId(), EV_READ)
            , server(server)
            , socket(std::move(so))
            , hit(hit)
        {}
        virtual void eventActivateNonBlocking() override
        {
            std::get<1>(hit) = true;
            server.transferHandler<SwapHandler>(hit, socket.getSocketId());
            server.flagShutDown();
            std::get<0>(hit) = true;
        }
};
class InHandlerTest: public ThorsAnvil::Nisse::Core::Service::HandlerSuspendable
{
    using CoRoutine = ThorsAnvil::Nisse::Core::Service::Context<short>::pull_type;
    using Yield     = ThorsAnvil::Nisse::Core::Service::Context<short>::push_type;

    ThorsAnvil::Nisse::Core::Service::Server&   server;
    ThorsAnvil::Nisse::Core::Socket::DataSocket socket;
    std::tuple<bool, std::function<void(ThorsAnvil::Nisse::Core::Service::Server&)>>&    active;

    public:
        InHandlerTest(ThorsAnvil::Nisse::Core::Service::Server& server, ThorsAnvil::Nisse::Core::Socket::DataSocket&& so, std::tuple<bool, std::function<void(ThorsAnvil::Nisse::Core::Service::Server&)>>& active)
            : HandlerSuspendable(server, so.getSocketId(), EV_READ)
            , server(server)
            , socket(std::move(so))
            , active(active)
        {}
        virtual void eventActivateNonBlocking() override
        {
            std::get<0>(active) = true;
            std::get<1>(active)(server);
            server.flagShutDown();
        }
};


#endif
