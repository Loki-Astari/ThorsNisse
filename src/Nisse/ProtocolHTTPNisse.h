#ifndef THORSANVIL_NISSE_PROTOCOL_HTTP_NISSE_H
#define THORSANVIL_NISSE_PROTOCOL_HTTP_NISSE_H

#include "NisseHandler.h"
#include <boost/coroutine/asymmetric_coroutine.hpp>
#include <map>
#include <vector>
#include "http_parser.h"

namespace ThorsAnvil
{
    namespace Nisse
    {

using HttpParser            = http_parser;
using HttpParserSettings    = http_parser_settings;

using CoRoutine = boost::coroutines::asymmetric_coroutine<void>::pull_type;
using Yield     = boost::coroutines::asymmetric_coroutine<void>::push_type;

class HTTPHandlerAccept: public NisseHandler
{
    private:
        ThorsAnvil::Socket::DataSocket                  socket;
        HttpParserSettings                              settings;
        HttpParser                                      parser;
        std::vector<char>                               buffer;
        std::string                                     method;
        std::string                                     uri;
        std::string                                     version;
        std::map<std::string, std::vector<std::string>> headers;

        static constexpr std::size_t bufferLen = 80 * 1024;

    public:
        HTTPHandlerAccept(NisseService& parent, LibEventBase* base, ThorsAnvil::Socket::DataSocket&& socket);
        void headerParser(Yield& yield);
        virtual void eventActivate(LibSocketId sockId, short eventType) override;

        void onHeadersComplete();
        void onMessageBegin();
        void onMessageComplete();
        void onUrl(char const* at, size_t length);
        void onStatus(char const* at, size_t length);
        void onHeaderField(char const* at, size_t length);
        void onHeaderValue(char const* at, size_t length);
        void onBody(char const* at, size_t length);
};

    }
}

#endif
