#include "ProtocolHTTPNisse.h"
#include "NisseService.h"

#include <iostream>

using namespace ThorsAnvil::Nisse;

int onHeadersComplete(HttpParser* parser)
{
    HTTPHandlerAccept* parent = reinterpret_cast<HTTPHandlerAccept*>(parser->data);
    parent->onHeadersComplete();
    return 0;
}
int onMessageBegin(HttpParser* parser)
{
    HTTPHandlerAccept* parent = reinterpret_cast<HTTPHandlerAccept*>(parser->data);
    parent->onMessageBegin();
    return 0;
}
int onMessageComplete(HttpParser* parser)
{
    HTTPHandlerAccept* parent = reinterpret_cast<HTTPHandlerAccept*>(parser->data);
    parent->onMessageComplete();
    return 0;
}

int onUrl(HttpParser* parser, const char *at, size_t length)
{
    HTTPHandlerAccept* parent = reinterpret_cast<HTTPHandlerAccept*>(parser->data);
    parent->onUrl(at, length);
    return 0;
}
int onStatus(HttpParser* parser, const char *at, size_t length)
{
    HTTPHandlerAccept* parent = reinterpret_cast<HTTPHandlerAccept*>(parser->data);
    parent->onStatus(at, length);
    return 0;
}
int onHeaderField(HttpParser* parser, const char *at, size_t length)
{
    HTTPHandlerAccept* parent = reinterpret_cast<HTTPHandlerAccept*>(parser->data);
    parent->onHeaderField(at, length);
    return 0;
}
int onHeaderValue(HttpParser* parser, const char *at, size_t length)
{
    HTTPHandlerAccept* parent = reinterpret_cast<HTTPHandlerAccept*>(parser->data);
    parent->onHeaderValue(at, length);
    return 0;
}
int onBody(HttpParser* parser, const char *at, size_t length)
{
    HTTPHandlerAccept* parent = reinterpret_cast<HTTPHandlerAccept*>(parser->data);
    parent->onBody(at, length);
    return 0;
}

HTTPHandlerAccept::HTTPHandlerAccept(NisseService& parent, LibEventBase* base, ThorsAnvil::Socket::DataSocket&& so)
    : NisseHandler(parent, base, so.getSocketId(), EV_READ)
    , socket(std::move(so))
    , buffer(bufferLen)
{
    settings.on_headers_complete    = ::onHeadersComplete;
    settings.on_message_begin       = ::onMessageBegin;
    settings.on_message_complete    = ::onMessageComplete;

    settings.on_header_field        = ::onHeaderField;
    settings.on_header_value        = ::onHeaderValue;
    settings.on_url                 = ::onUrl;
    settings.on_status              = ::onStatus;
    settings.on_body                = ::onBody;

    http_parser_init(&parser, HTTP_REQUEST);
    parser.data                 = this;

}

/*
        std::string                                     method;
        std::string                                     uri;
        std::string                                     version
        std::map<std::string, std::vector<std::string>> headers;
*/
void HTTPHandlerAccept::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    std::cerr << "eventActivate:\n";
    std::size_t recved  = socket.getMessageData(&buffer[0], bufferLen, 0);
    std::cerr << "rec: " << recved << "\n";
    std::size_t nparsed = http_parser_execute(&parser, &settings, &buffer[0], recved);
    std::cerr << "npar: " << nparsed << "\n";

    if (parser.upgrade)
    {
        /* handle new protocol */
    }
    else if (nparsed != recved)
    {
        /* Handle error. Usually just close the connection. */
    }

    if (recved == 0)
    {
        std::cerr << "No More Data\n";
        eventListener.drop();
        parent.delHandler(this);
    }
}

void HTTPHandlerAccept::onHeadersComplete()
{
    std::cerr << "onHeadersComplete\n";
}
void HTTPHandlerAccept::onMessageBegin()
{
    std::cerr << "onMessageBegin\n";
}
void HTTPHandlerAccept::onMessageComplete()
{
    std::cerr << "onMessageComplete\n";
}
void HTTPHandlerAccept::onUrl(char const* at, size_t length)
{
    std::cerr << "onURL: " << std::string(at, at + length) << "\n";
    std::cerr << "Type: " << parser.type << " -> ";
    switch (parser.type)
    {
        case HTTP_REQUEST:      std::cerr << "HTTP_REQUEST\n";break;
        case HTTP_RESPONSE:     std::cerr << "HTTP_RESPONSE\n";break;
        case HTTP_BOTH:         std::cerr << "HTTP_BOTH\n";break;
        default:
            std::cerr << "Uknown\n";
    }
    std::cerr << "Method: " << parser.method << " -> ";
    switch (parser.method)
    {
        case HTTP_DELETE:       std::cerr << "DELETE\n";break;
        case HTTP_GET:          std::cerr << "GET\n";break;
        case HTTP_HEAD:         std::cerr << "HEAD\n";break;
        case HTTP_POST:         std::cerr << "POST\n";break;
        case HTTP_PUT:          std::cerr << "PUT\n";break;
        default:
            std::cerr << "Unknown\n";
    }

    uri.assign(at, length);
}
void HTTPHandlerAccept::onStatus(char const* at, size_t length)
{
    std::cerr << "onStatus: " << std::string(at, at + length) << "\n";
}
void HTTPHandlerAccept::onHeaderField(char const* at, size_t length)
{
    std::cerr << "onHeaderField: " << std::string(at, at + length) << "\n";
}
void HTTPHandlerAccept::onHeaderValue(char const* at, size_t length)
{
    std::cerr << "onHeaderValue: " << std::string(at, at + length) << "\n";
}
void HTTPHandlerAccept::onBody(char const* at, size_t length)
{
    std::cerr << "onBody: " << std::string(at, at + length) << "\n";
}
