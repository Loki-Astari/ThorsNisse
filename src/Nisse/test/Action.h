#ifndef THORSANVIL_NISSE_TEST_ACTION_H
#define THORSANVIL_NISSE_TEST_ACTION_H

#include "../NisseHandler.h"
#include "ThorsNisseSocket/Socket.h"

class Action: public ThorsAnvil::Nisse::NisseHandler
{
	ThorsAnvil::Nisse::NisseService&	service;
    ThorsAnvil::Socket::DataSocket      dataSocket;
    public:
        Action(ThorsAnvil::Nisse::NisseService& parent, ThorsAnvil::Socket::DataSocket&& ds)
			: NisseHandler(parent, ds.getSocketId(), EV_READ)
            , service(parent)
            , dataSocket(std::move(ds))
		{}
        virtual void eventActivate(ThorsAnvil::Nisse::LibSocketId /*sockId*/, short /*eventType*/) override
		{
			service.flagShutDown();
		}
};
class ActionUnReg: public ThorsAnvil::Nisse::NisseHandler
{
	ThorsAnvil::Nisse::NisseService&	service;
    ThorsAnvil::Socket::DataSocket      dataSocket;
    public:
        ActionUnReg(ThorsAnvil::Nisse::NisseService& parent, ThorsAnvil::Socket::DataSocket&& ds)
			: NisseHandler(parent, ds.getSocketId(), EV_READ)
            , service(parent)
            , dataSocket(std::move(ds))
		{}
        virtual void eventActivate(ThorsAnvil::Nisse::LibSocketId /*sockId*/, short /*eventType*/) override
		{
			service.flagShutDown();
            dropHandler();
		}
};

#endif
