#ifndef THORSANVIL_NISSE_PROTOCOL_HTTP_HTTP_PROTOCOL_H
#define THORSANVIL_NISSE_PROTOCOL_HTTP_HTTP_PROTOCOL_H

#include "Binder.h"
#include "Types.h"
#include "HttpScanner.h"
#include "ThorsNisseCoreService/Handler.h"
#include <cstddef>

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace Protocol
        {
            namespace HTTP
            {

class ReadRequestHandler: public Core::Service::HandlerSuspendable
{
    using DataSocket = ThorsAnvil::Nisse::Core::Socket::DataSocket;
    private:
        Core::Socket::DataSocket    socket;
        Binder const&               binder;
        Response*                   flusher;
        bool                        running;

        static constexpr std::size_t bufferLen = 80 * 1024;

    public:
        ReadRequestHandler(Core::Service::Server& parent, Core::Socket::DataSocket&& socket, Binder const& binder);
        virtual void eventActivateNonBlocking() override;
        void setFlusher(Response* f){flusher = f;}
        void flushing()             {if (flusher){flusher->flushing();}}
};

            }
        }
    }
}

#endif
