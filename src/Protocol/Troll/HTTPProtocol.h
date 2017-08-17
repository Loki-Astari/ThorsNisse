#ifndef THORSANVIL_NISSE_PROTOCOL_HTTP_HANDLERS_H
#define THORSANVIL_NISSE_PROTOCOL_HTTP_HANDLERS_H

#include "Binder.h"
#include "Types.h"
#include "HttpScanner.h"
#include "ThorsNisseCoreService/NisseHandler.h"
#include <cstddef>

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace ProtocolHTTP
        {

class ReadRequestHandler: public Core::Service::NisseHandler
{
    using DataSocket = ThorsAnvil::Nisse::Core::Socket::DataSocket;
    private:
        Response*           flusher;
        Yield*              yield;
        bool                running;
        CoRoutine           worker;

        static constexpr std::size_t bufferLen = 80 * 1024;

    public:
        ReadRequestHandler(Core::Service::NisseService& parent, Core::Socket::DataSocket&& socket, Binder const& binder);
        virtual short eventActivate(Core::Service::LibSocketId sockId, short eventType) override;
        void setFlusher(Response* f){flusher = f;}
        void flushing()             {if (flusher){flusher->flushing();}}
        void setYield(Yield& y)     {yield = &y;}
    private:
        virtual void suspend() override;
};

        }
    }
}

#endif
