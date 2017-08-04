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
        DataSocket              socket;
        Binder const&           binder;
        std::vector<char>       buffer;
        std::string             version;
        HttpScanner             scanner;

        static constexpr std::size_t bufferLen = 80 * 1024;
        virtual void requestComplete(
                                DataSocket&&    socket,
                                Binder const&   binder,
                                Method          method,
                                std::string&&   uri,
                                Headers&&       headers,
                                std::vector<char>&& buffer, char const* bodyBegin, char const* bodyEnd);

    public:
        ReadRequestHandler(NisseService& parent, ThorsAnvil::Socket::DataSocket&& socket, Binder const& binder);
        void headerParser(Yield& yield);
        virtual short eventActivate(LibSocketId sockId, short eventType) override;
    private:
};

class WriteResponseHandler: public NisseHandler
{
    using DataSocket = ThorsAnvil::Socket::DataSocket;
    private:
        Response*               flusher;
        CoRoutine               worker;
    public:
        WriteResponseHandler(NisseService& parent,
                             ThorsAnvil::Socket::DataSocket&& socket,
                             Binder const& binder,
                             Method method,
                             std::string&& uri,
                             Headers&& headers,
                             std::vector<char>&& buffer,
                             char const* bodyBegin,
                             char const* bodyEnd);
        void setFlusher(Response* f){flusher = f;}
        void flushing()             {if (flusher){flusher->flushing();}}
        virtual short eventActivate(LibSocketId sockId, short eventType) override;
};

        }
    }
}

#endif
