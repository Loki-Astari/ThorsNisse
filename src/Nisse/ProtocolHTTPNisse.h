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

enum class HttpMethod {Get, Put, Post, Delete, Head};
using Headers = std::map<std::string, std::vector<std::string>>;

class HTTPHandlerAccept: public NisseHandler
{
    using DataSocket = ThorsAnvil::Socket::DataSocket;
    private:
        DataSocket              socket;
        HttpParserSettings      settings;
        HttpParser              parser;
        std::vector<char>       buffer;
        HttpMethod              method;
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
        HTTPHandlerAccept(NisseService& parent, LibEventBase* base, ThorsAnvil::Socket::DataSocket&& socket);
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

class SocketStream: public std::istream
{
    public:
        SocketStream(std::vector<char>&&, char const*, char const*)
            : std::istream(nullptr)
        {}
};
class HTTPHandlerRunResource: public NisseHandler
{
    using DataSocket = ThorsAnvil::Socket::DataSocket;
    private:
        DataSocket              socket;
        HttpMethod              method;
        std::string             uri;
        Headers                 headers;
        SocketStream            inputStream;
        std::size_t             alreadyPut;
        std::string             message;
    public:
        HTTPHandlerRunResource(NisseService& parent, LibEventBase* base, ThorsAnvil::Socket::DataSocket&& so,
                                std::vector<char>&& buffer, char const* bodyBegin, char const* bodyEnd,
                                HttpMethod method, std::string&& uri,
                                Headers&& headers);
        virtual void eventActivate(LibSocketId sockId, short eventType) override;
    private:
        std::string getTimeString();
        std::string buildMessage();
};


    }
}

#endif
