#include "ProtocolHTTPNisse.h"
#include "NisseService.h"

using namespace ThorsAnvil::Nisse::ProtocolHTTP;

int onHeadersComplete(HttpParser* parser)
{
    ReadRequestHandler* parent = reinterpret_cast<ReadRequestHandler*>(parser->data);
    parent->onHeadersComplete();
    return 0;
}
int onMessageBegin(HttpParser* parser)
{
    ReadRequestHandler* parent = reinterpret_cast<ReadRequestHandler*>(parser->data);
    parent->onMessageBegin();
    return 0;
}
int onMessageComplete(HttpParser* parser)
{
    ReadRequestHandler* parent = reinterpret_cast<ReadRequestHandler*>(parser->data);
    parent->onMessageComplete();
    return 0;
}

int onUrl(HttpParser* parser, const char *at, std::size_t length)
{
    ReadRequestHandler* parent = reinterpret_cast<ReadRequestHandler*>(parser->data);
    parent->onUrl(at, length);
    return 0;
}
int onStatus(HttpParser* parser, const char *at, std::size_t length)
{
    ReadRequestHandler* parent = reinterpret_cast<ReadRequestHandler*>(parser->data);
    parent->onStatus(at, length);
    return 0;
}
int onHeaderField(HttpParser* parser, const char *at, std::size_t length)
{
    ReadRequestHandler* parent = reinterpret_cast<ReadRequestHandler*>(parser->data);
    parent->onHeaderField(at, length);
    return 0;
}
int onHeaderValue(HttpParser* parser, const char *at, std::size_t length)
{
    ReadRequestHandler* parent = reinterpret_cast<ReadRequestHandler*>(parser->data);
    parent->onHeaderValue(at, length);
    return 0;
}
int onBody(HttpParser* parser, const char *at, std::size_t length)
{
    ReadRequestHandler* parent = reinterpret_cast<ReadRequestHandler*>(parser->data);
    parent->onBody(at, length);
    return 0;
}

ReadRequestHandler::ReadRequestHandler(NisseService& parent, LibEventBase* base, ThorsAnvil::Socket::DataSocket&& so, Binder const& binder)
    : NisseHandler(parent, base, so.getSocketId(), EV_READ)
    , socket(std::move(so))
    , binder(binder)
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
void ReadRequestHandler::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    bool        more;
    std::size_t recved;
    std::tie(more, recved) = socket.getMessageData(&buffer[0], bufferLen, 0);
    std::size_t nparsed = http_parser_execute(&parser, &settings, &buffer[0], recved);

    if (parser.upgrade)
    {
        /* handle new protocol */
    }
    else if (nparsed != recved)
    {
        /* Handle error. Usually just close the connection. */
    }

    if (!more)
    {
        dropHandler();
    }
}

void ReadRequestHandler::onHeadersComplete()
{
    addCurrentHeader();
    moveHandler<WriteResponseHandler>(std::move(socket),
                                      binder,
                                      method,
                                      std::move(uri),
                                      std::move(headers),
                                      std::move(buffer),
                                      bodyBegin, bodyEnd
                                     );
    //std::move(buffer), bodyBegin, bodyEnd, method, std::move(uri), std::move(headers));
}
void ReadRequestHandler::onMessageBegin()
{
}
void ReadRequestHandler::onMessageComplete()
{
}
void ReadRequestHandler::onUrl(char const* at, std::size_t length)
{
    switch (parser.method)
    {
        case HTTP_DELETE:       method = Method::Delete;break;
        case HTTP_GET:          method = Method::Get;   break;
        case HTTP_HEAD:         method = Method::Head;  break;
        case HTTP_POST:         method = Method::Post;  break;
        case HTTP_PUT:          method = Method::Put;   break;
        default:
            throw std::runtime_error("ThorsAnvil::Nisse::ReadRequestHandler::onUrl: unknown HTTP Method");
    }

    uri.assign(at, length);
    gotValue    = false;
}
void ReadRequestHandler::onStatus(char const* /*at*/, std::size_t /*length*/)
{
}
void ReadRequestHandler::onHeaderField(char const* at, std::size_t length)
{
    addCurrentHeader();
    currentHead.append(at, length);
}
void ReadRequestHandler::onHeaderValue(char const* at, std::size_t length)
{
    gotValue = true;
    currentValue.append(at, length);
}
void ReadRequestHandler::onBody(char const* at, std::size_t length)
{
    bodyBegin   = at;
    bodyEnd     = at + length;
}

void ReadRequestHandler::addCurrentHeader()
{
    if (gotValue)
    {
        headers[currentHead].emplace_back(std::move(currentValue));
        gotValue = false;
        currentHead.clear();
        currentValue.clear();
    }
}

// ------------------

WriteResponseHandler::WriteResponseHandler(NisseService& parent, LibEventBase* base,
                                           ThorsAnvil::Socket::DataSocket&& so,
                                           Binder const& binder,
                                           Method methodParam,
                                           std::string&& uriParam,
                                           Headers&& headersParam,
                                           std::vector<char>&& bufferParam,
                                           char const* bodyBeginParam,
                                           char const* bodyEndParam)
    : NisseHandler(parent, base, so.getSocketId(), EV_WRITE)
    , worker([  socket      = std::move(so),
                &action     = binder.find(uriParam),
                method      = methodParam,
                uri         = std::move(uriParam),
                headers     = std::move(headersParam),
                buffer      = std::move(bufferParam),
                bodyBegin   = bodyBeginParam,
                bodyEnd     = bodyEndParam
             ](Yield& yield) mutable
                {
                    Request     request(socket, yield, method, URI(std::move(uri)), std::move(headers), std::move(buffer), bodyBegin, bodyEnd);
                    Response    response(socket, yield);
                    yield();
                    action(request, response);
                }
            )
{}

void WriteResponseHandler::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    worker();
    if (!worker)
    {
        dropHandler();
    }
}

