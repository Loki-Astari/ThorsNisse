#ifndef THORSANVIL_NISSE_PROTOCOL_HTTP_NISSE_H
#define THORSANVIL_NISSE_PROTOCOL_HTTP_NISSE_H

#include "NisseHandler.h"
#include <boost/coroutine/asymmetric_coroutine.hpp>

namespace ThorsAnvil
{
    namespace Nisse
    {

class HTTPHandlerAccept: public NisseHandler
{
    private:
        ThorsAnvil::Socket::DataSocket      socket;
    public:
        HTTPHandlerAccept(NisseService& parent, LibEventBase* base, ThorsAnvil::Socket::DataSocket&& socket);
        virtual void eventActivate(LibSocketId sockId, short eventType) override;
};

    }
}

#endif
