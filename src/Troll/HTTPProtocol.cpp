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
    , data(parser)
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
    parser.data                     = this;

}

short ReadRequestHandler::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
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

    if (data.messageComplete)
    {
        requestComplete(std::move(socket),
                        binder,
                        data.method,
                        std::move(data.uri),
                        std::move(data.headers),
                        std::move(buffer),
                        data.bodyBegin, data.bodyEnd
                       );
        return 0;
    }
    else if (!more)
    {
        dropHandler();
        return 0;
    }
    return EV_READ;
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
    data.addCurrentHeader();
    data.messageComplete = true;
}
void ReadRequestHandler::onMessageBegin()
{
}
void ReadRequestHandler::onMessageComplete()
{
}
void ReadRequestHandler::onUrl(char const* at, std::size_t length)
{
    switch (data.parser.method)
    {
        case HTTP_DELETE:       data.method = Method::Delete;break;
        case HTTP_GET:          data.method = Method::Get;   break;
        case HTTP_HEAD:         data.method = Method::Head;  break;
        case HTTP_POST:         data.method = Method::Post;  break;
        case HTTP_PUT:          data.method = Method::Put;   break;
        default:
            throw std::runtime_error("ThorsAnvil::Nisse::ReadRequestHandler::onUrl: unknown HTTP Method");
    }

    data.uri.assign(at, length);
    data.gotValue    = false;
}
void ReadRequestHandler::onStatus(char const* /*at*/, std::size_t /*length*/)
{
}
void ReadRequestHandler::onHeaderField(char const* at, std::size_t length)
{
    data.addCurrentHeader();
    data.currentHead.append(at, length);
}
void ReadRequestHandler::onHeaderValue(char const* at, std::size_t length)
{
    data.gotValue = true;
    data.currentValue.append(at, length);
}
void ReadRequestHandler::onBody(char const* at, std::size_t length)
{
    data.bodyBegin   = at;
    data.bodyEnd     = at + length;
}

// ------------------

class DevNullStreamBuf: public std::streambuf
{
    private:
        typedef std::streambuf::traits_type traits;
        typedef traits::int_type            int_type;
        typedef traits::char_type           char_type;

        std::size_t written;

    protected:
        virtual std::streamsize xsputn(char_type const* /*s*/,std::streamsize count) override
        {
            written += count;
            return count;
        }

        virtual std::streampos seekoff(std::streamoff /*off*/,
                                       std::ios_base::seekdir /*way*/,
                                       std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override
        {
            ((void)which);
            return written;
        }
    public:
        DevNullStreamBuf()
            : written(0)
        {}
};

WriteResponseHandler::WriteResponseHandler(NisseService& parent,
                                           ThorsAnvil::Socket::DataSocket&& so,
                                           Binder const& binder,
                                           Method methodParam,
                                           std::string&& uriParam,
                                           Headers&& headersParam,
                                           std::vector<char>&& bufferParam,
                                           char const* bodyBeginParam,
                                           char const* bodyEndParam)
    : NisseHandler(parent, so.getSocketId(), EV_WRITE | EV_READ)
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
                    Action const& action(binder.find(method, uri.host, uri.path));
                    Socket::ISocketStream   input(socket, [&yield](){yield(EV_READ);}, [](){}, std::move(buffer), bodyBegin, bodyEnd);
                    Socket::OSocketStream   output(socket, [&yield](){yield(EV_WRITE);}, [&parent](){parent.flushing();});
                    DevNullStreamBuf        devNullBuffer;
                    if (method == Method::Head)
                    {
                        output.rdbuf(&devNullBuffer);
                    }

                    Request       request(method, URI(std::move(uri)), headers, input);
                    Response      response(parent, socket, output);
                    yield(EV_READ | EV_WRITE);
                    action(request, response);
                }
            )
{}

short WriteResponseHandler::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    worker();
    if (!worker)
    {
        dropHandler();
        return 0;
    }
    return worker.get();
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
