#include "ProtocolHTTPNisse.h"
#include "NisseService.h"

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

int onUrl(HttpParser* parser, const char *at, std::size_t length)
{
    HTTPHandlerAccept* parent = reinterpret_cast<HTTPHandlerAccept*>(parser->data);
    parent->onUrl(at, length);
    return 0;
}
int onStatus(HttpParser* parser, const char *at, std::size_t length)
{
    HTTPHandlerAccept* parent = reinterpret_cast<HTTPHandlerAccept*>(parser->data);
    parent->onStatus(at, length);
    return 0;
}
int onHeaderField(HttpParser* parser, const char *at, std::size_t length)
{
    HTTPHandlerAccept* parent = reinterpret_cast<HTTPHandlerAccept*>(parser->data);
    parent->onHeaderField(at, length);
    return 0;
}
int onHeaderValue(HttpParser* parser, const char *at, std::size_t length)
{
    HTTPHandlerAccept* parent = reinterpret_cast<HTTPHandlerAccept*>(parser->data);
    parent->onHeaderValue(at, length);
    return 0;
}
int onBody(HttpParser* parser, const char *at, std::size_t length)
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
    std::size_t recved  = socket.getMessageData(&buffer[0], bufferLen, 0);
    std::size_t nparsed = http_parser_execute(&parser, &settings, &buffer[0], recved);

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
        eventListener.drop();
        parent.delHandler(this);
    }
}

void HTTPHandlerAccept::onHeadersComplete()
{
    addCurrentHeader();
    parent.addHandler<HTTPHandlerRunResource>(std::move(socket), std::move(buffer), bodyBegin, bodyEnd, method, std::move(uri), std::move(headers));
    eventListener.drop();
    parent.delHandler(this);
}
void HTTPHandlerAccept::onMessageBegin()
{
}
void HTTPHandlerAccept::onMessageComplete()
{
}
void HTTPHandlerAccept::onUrl(char const* at, std::size_t length)
{
    switch (parser.method)
    {
        case HTTP_DELETE:       method = HttpMethod::Delete;break;
        case HTTP_GET:          method = HttpMethod::Get;   break;
        case HTTP_HEAD:         method = HttpMethod::Head;  break;
        case HTTP_POST:         method = HttpMethod::Post;  break;
        case HTTP_PUT:          method = HttpMethod::Put;   break;
        default:
            throw std::runtime_error("ThorsAnvil::Nisse::HTTPHandlerAccept::onUrl: unknown HTTP Method");
    }

    uri.assign(at, length);
    gotValue    = false;
}
void HTTPHandlerAccept::onStatus(char const* /*at*/, std::size_t /*length*/)
{
}
void HTTPHandlerAccept::onHeaderField(char const* at, std::size_t length)
{
    addCurrentHeader();
    currentHead.append(at, length);
}
void HTTPHandlerAccept::onHeaderValue(char const* at, std::size_t length)
{
    gotValue = true;
    currentValue.append(at, length);
}
void HTTPHandlerAccept::onBody(char const* at, std::size_t length)
{
    bodyBegin   = at;
    bodyEnd     = at + length;
}

void HTTPHandlerAccept::addCurrentHeader()
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

HTTPHandlerRunResource::HTTPHandlerRunResource(NisseService& parent, LibEventBase* base, ThorsAnvil::Socket::DataSocket&& so,
                                                std::vector<char>&& buffer, char const* bodyBegin, char const* bodyEnd,
                                                HttpMethod method, std::string&& uri,
                                                Headers&& headers)
    : NisseHandler(parent, base, so.getSocketId(), EV_WRITE)
    , socket(std::move(so))
    , method(method)
    , uri(std::move(uri))
    , headers(std::move(headers))
    , inputStream(std::move(buffer), bodyBegin, bodyEnd)
    , alreadyPut(0)
    , message(buildMessage())
{
    this->method = HttpMethod::Get;
}

void HTTPHandlerRunResource::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    alreadyPut += socket.putMessageData(message.c_str(), message.size(), alreadyPut);
    if (alreadyPut == message.size())
    {
        eventListener.drop();
        parent.delHandler(this);
    }
}

class TimePrinter
{
    private:
        char const* time;
    public:
        TimePrinter()
            : time(getTimeString())
        {}
        friend std::ostream& operator<<(std::ostream& str, TimePrinter const& data)
        {
            str.write(data.time, 24);
            return str;
        }
    private:
        static char const* getTimeString()
        {
            using TimeT = std::time_t;
            using TimeI = std::tm;

            TimeT   rawtime;
            TimeI*  timeinfo;

            ::time(&rawtime);
            timeinfo = ::localtime(&rawtime);

            return ::asctime(timeinfo);
        }
};

std::string HTTPHandlerRunResource::buildMessage()
{
    std::stringstream messageStream;
    messageStream << "HTTP/1.1 404 Not Found\r\n"
                  << "Date: " << TimePrinter() << "\r\n"
                  << "Server: Nisse\r\n"
                  << "Content-Length: 44\r\n"
                  << "Content-Type: text/html\r\n"
                  << "Connection: Closed\r\n"
                  << "\r\n"
                  << "<html><body><h1>Not Found</h1></body></html>";
    return messageStream.str();
}
