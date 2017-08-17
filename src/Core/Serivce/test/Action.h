#ifndef THORSANVIL_NISSE_TEST_ACTION_H
#define THORSANVIL_NISSE_TEST_ACTION_H

#include "../NisseHandler.h"
#include "ThorsNisseSocket/Socket.h"

class Action: public ThorsAnvil::Nisse::Core::Service::NisseHandler
{
    ThorsAnvil::Nisse::Core::Service::NisseService&     service;
    ThorsAnvil::Socket::DataSocket      dataSocket;
    public:
        Action(ThorsAnvil::Nisse::Core::Service::NisseService& parent, ThorsAnvil::Socket::DataSocket&& ds)
            : NisseHandler(parent, ds.getSocketId(), EV_READ | EV_PERSIST)
            , service(parent)
            , dataSocket(std::move(ds))
        {}
        virtual short eventActivate(ThorsAnvil::Nisse::Core::Service::LibSocketId /*sockId*/, short /*eventType*/) override
        {
            service.flagShutDown();
            return 0;
        }
};
class ActionUnReg: public ThorsAnvil::Nisse::Core::Service::NisseHandler
{
    ThorsAnvil::Nisse::Core::Service::NisseService&     service;
    ThorsAnvil::Socket::DataSocket      dataSocket;
    public:
        ActionUnReg(ThorsAnvil::Nisse::Core::Service::NisseService& parent, ThorsAnvil::Socket::DataSocket&& ds)
            : NisseHandler(parent, ds.getSocketId(), EV_READ | EV_PERSIST)
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

#endif
