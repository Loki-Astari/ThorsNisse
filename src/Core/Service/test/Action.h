#ifndef THORSANVIL_NISSE_TEST_ACTION_H
#define THORSANVIL_NISSE_TEST_ACTION_H

#include "../Server.h"
#include "../Handler.h"
#include "../CoRoutine.h"
#include "ThorsNisseCoreSocket/Socket.h"
#include <iostream>

class Action: public ThorsAnvil::Nisse::Core::Service::Handler
{
    ThorsAnvil::Nisse::Core::Service::Server&           service;
    ThorsAnvil::Nisse::Core::Socket::DataSocket         dataSocket;
    public:
        Action(ThorsAnvil::Nisse::Core::Service::Server& parent, ThorsAnvil::Nisse::Core::Socket::DataSocket&& ds)
            : Handler(parent, ds.getSocketId(), EV_READ | EV_PERSIST)
            , service(parent)
            , dataSocket(std::move(ds))
        {}
        virtual short eventActivate(ThorsAnvil::Nisse::Core::Service::LibSocketId /*sockId*/, short /*eventType*/) override
        {
            service.flagShutDown();
            return 0;
        }
        virtual bool  blocking()  override {return false;}
};
class ActionUnReg: public ThorsAnvil::Nisse::Core::Service::Handler
{
    ThorsAnvil::Nisse::Core::Service::Server&           service;
    ThorsAnvil::Nisse::Core::Socket::DataSocket         dataSocket;
    public:
        ActionUnReg(ThorsAnvil::Nisse::Core::Service::Server& parent, ThorsAnvil::Nisse::Core::Socket::DataSocket&& ds)
            : Handler(parent, ds.getSocketId(), EV_READ | EV_PERSIST)
            , service(parent)
            , dataSocket(std::move(ds))
        {}
        virtual short eventActivate(ThorsAnvil::Nisse::Core::Service::LibSocketId /*sockId*/, short /*eventType*/) override
        {
            service.flagShutDown();
            dropHandler();
            return 0;
        }
        virtual bool  blocking()  override {return false;}
};
class SwapHandler: public ThorsAnvil::Nisse::Core::Service::Handler
{
    std::tuple<bool, bool, bool>& hit;
    public:
        SwapHandler(ThorsAnvil::Nisse::Core::Service::Server& server, std::tuple<bool, bool, bool>& hit, short socketId)
            : Handler(server, socketId, EV_READ)
            , hit(hit)
        {}
        virtual short eventActivate(ThorsAnvil::Nisse::Core::Service::LibSocketId, short)
        {
            std::get<2>(hit) = true;
            dropHandler();
            return 0;
        }
        virtual bool  blocking()
        {
            return true;
        }
};

class TestHandler: public ThorsAnvil::Nisse::Core::Service::Handler
{
    using CoRoutine = ThorsAnvil::Nisse::Core::Service::Context<short>::pull_type;
    using Yield     = ThorsAnvil::Nisse::Core::Service::Context<short>::push_type;

    ThorsAnvil::Nisse::Core::Socket::DataSocket socket;
    std::tuple<bool, bool, bool>&               hit;
    Yield*                                      yield;
    CoRoutine                                   worker;

    public:
        TestHandler(ThorsAnvil::Nisse::Core::Service::Server& server, ThorsAnvil::Nisse::Core::Socket::DataSocket&& so, std::tuple<bool, bool, bool>& hit)
            : Handler(server, so.getSocketId(), EV_READ)
            , socket(std::move(so))
            , hit(hit)
            , worker([&server, &parentYield = this->yield, &hit, socketId = socket.getSocketId()](Yield& yield)
                {
                    parentYield = &yield;
                    yield(EV_READ);
                    server.transferHandler<SwapHandler>(hit, socketId);
                    server.flagShutDown();
                    std::get<0>(hit) = true;
                    return 0;
                }
              )
        {}
        virtual short eventActivate(ThorsAnvil::Nisse::Core::Service::LibSocketId, short) override
        {
            std::get<1>(hit) = true;
            if (!worker())
            {
                dropHandler();
                return 0;
            }
            int result = worker.get();
            return result;
        }
        virtual bool  blocking() override
        {
            return false;
        }
        virtual void suspend() override
        {
            (*yield)(0);
        }
};


#endif
