#ifndef THORSANVIL_NISSE_PROTOCOL_HTTP_HANDLERS_H
#define THORSANVIL_NISSE_PROTOCOL_HTTP_HANDLERS_H

#include "Binder.h"
#include "Types.h"
#include "HttpScanner.h"
#include "ThorsNisse/NisseHandler.h"
#include <map>
#include <vector>

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace ProtocolHTTP
        {

class ReadRequestHandler: public NisseHandler
{
    using DataSocket = ThorsAnvil::Socket::DataSocket;
    private:
        Response*           flusher;
        CoRoutine           worker;

        static constexpr std::size_t bufferLen = 80 * 1024;

    public:
        ReadRequestHandler(NisseService& parent, ThorsAnvil::Socket::DataSocket&& socket, Binder const& binder);
        virtual short eventActivate(LibSocketId sockId, short eventType) override;
        void setFlusher(Response* f){flusher = f;}
        void flushing()             {if (flusher){flusher->flushing();}}
    private:
};

        }
    }
}

#endif
