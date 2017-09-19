#ifndef THORSANVIL_NISSE_PROTOCOL_HTTP_HTTP_PROTOCOL_H
#define THORSANVIL_NISSE_PROTOCOL_HTTP_HTTP_PROTOCOL_H

#include "Binder.h"
#include "Types.h"
#include "HttpScanner.h"
#include "ThorsNisseCoreService/Handler.h"
#include "ThorsNisseCoreService/ServerHandler.h"
#include <cstddef>

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace Protocol
        {
            namespace HTTP
            {

class ReadRequestHandler: public Core::Service::HandlerSuspendable<Core::Socket::DataSocket>
{
    private:
        Binder const&               binder;
        Response*                   flusher;
        /*MIY TODO remove*/bool                        running;

        static constexpr std::size_t bufferLen = 80 * 1024;

    public:
        ReadRequestHandler(Core::Service::Server& parent, Core::Socket::DataSocket&& socket, Binder const& binder);
        virtual bool eventActivateNonBlocking() override;
        void setFlusher(Response* f){flusher = f;}
        void flushing()             {if (flusher){flusher->flushing();}}
};

            }
        }
    }
}

#endif
