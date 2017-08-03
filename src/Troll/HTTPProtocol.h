#ifndef THORSANVIL_NISSE_PROTOCOL_HTTP_HANDLERS_H
#define THORSANVIL_NISSE_PROTOCOL_HTTP_HANDLERS_H

#include "Binder.h"
#include "Types.h"
#include "ThorsNisse/NisseHandler.h"
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
struct HttpParserData
{
    HttpParserData(HttpParser& parser)
        : parser(parser)
        , bodyBegin(nullptr)
        , bodyEnd(nullptr)
        , messageComplete(false)
        , gotValue(false)
    {}
    void addCurrentHeader()
    {
        if (gotValue)
        {
            headers[currentHead]    = std::move(currentValue);
            gotValue = false;
            currentHead.clear();
            currentValue.clear();
        }
    }

        HttpParser&             parser;
        Headers                 headers;
        std::string             currentHead;
        std::string             currentValue;
        std::string             uri;
        char const*             bodyBegin;
        char const*             bodyEnd;
        Method                  method;
        bool                    messageComplete;
        bool                    gotValue;
};

class ReadRequestHandler: public NisseHandler
{
    using DataSocket = ThorsAnvil::Socket::DataSocket;
    private:
        DataSocket              socket;
        Binder const&           binder;
        HttpParserSettings      settings;
        HttpParser              parser;
        HttpParserData          data;
        std::vector<char>       buffer;
        std::string             version;

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

        void onHeadersComplete();
        void onMessageBegin();
        void onMessageComplete();
        void onUrl(char const* at, std::size_t length);
        void onStatus(char const* at, std::size_t length);
        void onHeaderField(char const* at, std::size_t length);
        void onHeaderValue(char const* at, std::size_t length);
        void onBody(char const* at, std::size_t length);

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
