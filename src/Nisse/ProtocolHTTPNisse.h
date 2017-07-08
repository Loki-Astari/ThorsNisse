#ifndef THORSANVIL_NISSE_PROTOCOL_HTTP_NISSE_H
#define THORSANVIL_NISSE_PROTOCOL_HTTP_NISSE_H

#include "NisseHandler.h"
#include "ProtocolHTTPBinder.h"
#include "ProtocolHTTPTypes.h"
#include <map>
#include <vector>
#include "http_parser.h"

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace ProtocolHTTP
        {

using HttpParser            = http_parser;
using HttpParserSettings    = http_parser_settings;

class ReadRequestHandler: public NisseHandler
{
    using DataSocket = ThorsAnvil::Socket::DataSocket;
    private:
        DataSocket              socket;
        Binder const&           binder;
        HttpParserSettings      settings;
        HttpParser              parser;
        std::vector<char>       buffer;
        Method                  method;
        std::string             uri;
        std::string             version;
        Headers                 headers;

        bool                    gotValue;
        std::string             currentHead;
        std::string             currentValue;
        char const*             bodyBegin;
        char const*             bodyEnd;

        static constexpr std::size_t bufferLen = 80 * 1024;

    public:
        ReadRequestHandler(NisseService& parent, LibEventBase* base, ThorsAnvil::Socket::DataSocket&& socket, Binder const& binder);
        void headerParser(Yield& yield);
        virtual void eventActivate(LibSocketId sockId, short eventType) override;

        void onHeadersComplete();
        void onMessageBegin();
        void onMessageComplete();
        void onUrl(char const* at, std::size_t length);
        void onStatus(char const* at, std::size_t length);
        void onHeaderField(char const* at, std::size_t length);
        void onHeaderValue(char const* at, std::size_t length);
        void onBody(char const* at, std::size_t length);

    private:
        void addCurrentHeader();
};

class WriteResponseHandler: public NisseHandler
{
    using DataSocket = ThorsAnvil::Socket::DataSocket;
    private:
        CoRoutine               worker;
        DataSocket              socket;
    public:
        WriteResponseHandler(NisseService& parent, LibEventBase* base,
                             ThorsAnvil::Socket::DataSocket&& socket,
                             Binder const& binder,
                             Method method,
                             std::string&& uri,
                             Headers&& headers,
                             std::vector<char>&& buffer,
                             char const* bodyBegin,
                             char const* bodyEnd);
        virtual void eventActivate(LibSocketId sockId, short eventType) override;
    private:
        static std::string buildMessage();
};

        }
    }
}

#endif
