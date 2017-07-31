#include "HTTPProtocol.h"
#include "ThorsNisse/NisseService.h"

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

ReadRequestHandler::ReadRequestHandler(NisseService& parent, ThorsAnvil::Socket::DataSocket&& so, Binder const& binder)
    : NisseHandler(parent, so.getSocketId(), EV_READ)
    , socket(std::move(so))
    , binder(binder)
    , buffer(bufferLen)
    , bodyBegin(nullptr)
    , bodyEnd(nullptr)
    , gotValue(false)
    , messageComplete(false)
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

    if (messageComplete)
    {
        requestComplete(std::move(socket),
                        binder,
                        method,
                        std::move(uri),
                        std::move(headers),
                        std::move(buffer),
                        bodyBegin, bodyEnd
                       );
    }
    else if (!more)
    {
        dropHandler();
    }
}

void ReadRequestHandler::requestComplete(
                     DataSocket&&    socket,
                     Binder const&   binder,
                     Method          method,
                     std::string&&   uri,
                     Headers&&       headers,
                     std::vector<char>&& buffer,
                     char const* bodyBegin, char const* bodyEnd)
{
    moveHandler<WriteResponseHandler>(std::move(socket),
                                      binder,
                                      method,
                                      std::move(uri),
                                      std::move(headers),
                                      std::move(buffer),
                                      bodyBegin, bodyEnd
                                     );
}

void ReadRequestHandler::onHeadersComplete()
{
    addCurrentHeader();
    messageComplete = true;
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
        headers[currentHead]    = std::move(currentValue);
        gotValue = false;
        currentHead.clear();
        currentValue.clear();
    }
}

// ------------------

WriteResponseHandler::WriteResponseHandler(NisseService& parent,
                                           ThorsAnvil::Socket::DataSocket&& so,
                                           Binder const& binder,
                                           Method methodParam,
                                           std::string&& uriParam,
                                           Headers&& headersParam,
                                           std::vector<char>&& bufferParam,
                                           char const* bodyBeginParam,
                                           char const* bodyEndParam)
    : NisseHandler(parent, so.getSocketId(), EV_WRITE)
    , flusher(nullptr)
    , worker([ &parent      = *this,
                socket      = std::move(so),
               &binder      = binder,
                method      = methodParam,
                uriParam    = std::move(uriParam),
                headers     = std::move(headersParam),
                buffer      = std::move(bufferParam),
                bodyBegin   = bodyBeginParam,
                bodyEnd     = bodyEndParam
             ](Yield& yield) mutable
                {
                    URI const     uri(headers.get("Host"), std::move(uriParam));
                    Action const& action(binder.find(Method::Get, uri.host, uri.path));
                    Socket::ISocketStream	input(socket, [&yield](){yield();}, [](){}, std::move(buffer), bodyBegin, bodyEnd);
                    Socket::OSocketStream   output(socket, [&yield](){yield();}, [&parent](){parent.flushing();});
                    Request       request(method, URI(std::move(uri)), headers, input);
                    Response      response(output);
                    parent.setFlusher(&response);
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

#ifdef COVERAGE_TEST
/*
 * This code is only compiled into the unit tests for code coverage purposes
 * It is not part of the live code.
 */
#include "ThorsNisse/NisseHandler.h"
#include "ThorsNisse/NisseService.tpp"
#include "ThorsNisse/NisseHandler.tpp"
#include "Types.h"
template void ThorsAnvil::Nisse::NisseHandler::moveHandler
    <WriteResponseHandler,
     ThorsAnvil::Socket::DataSocket,
     Binder const&,
     Method,
     std::string,
     Headers,
     std::vector<char>,
     char const*,
     char const*>
    (ThorsAnvil::Socket::DataSocket&&,
     Binder const&,
     Method&&,
     std::string&&,
     Headers&&,
     std::vector<char>&&,
     char const*&&,
     char const*&&);


template void ThorsAnvil::Nisse::NisseService::addHandler<WriteResponseHandler, ThorsAnvil::Socket::DataSocket, Binder const&, Method, std::string, Headers, std::vector<char>, char const*, char const*>(ThorsAnvil::Socket::DataSocket&&, Binder const&, Method&&, std::string&&, Headers&&, std::vector<char>&&, char const*&&, char const*&&);
#endif
