#include "HttpScanner.h"

using namespace ThorsAnvil::Nisse::ProtocolHTTP;

int onHeadersComplete(HttpParser* parser)
{
    HttpParserData* data = reinterpret_cast<HttpParserData*>(parser->data);
    data->addCurrentHeader();
    data->messageComplete = true;
    return 0;
}

int onMessageBegin(HttpParser* /*parser*/)
{
    return 0;
}

int onMessageComplete(HttpParser* /*parser*/)
{
    return 0;
}

int onUrl(HttpParser* parser, const char *at, std::size_t length)
{
    HttpParserData* data = reinterpret_cast<HttpParserData*>(parser->data);
    switch (parser->method)
    {
        case HTTP_DELETE:       data->method = Method::Delete;break;
        case HTTP_GET:          data->method = Method::Get;   break;
        case HTTP_HEAD:         data->method = Method::Head;  break;
        case HTTP_POST:         data->method = Method::Post;  break;
        case HTTP_PUT:          data->method = Method::Put;   break;
        default:
            throw std::runtime_error("ThorsAnvil::Nisse::ReadRequestHandler::onUrl: unknown HTTP Method");
    }

    data->uri.assign(at, length);
    data->gotValue    = false;
    return 0;
}

int onStatus(HttpParser* /*parser*/, const char* /*at*/, std::size_t /*length*/)
{
    return 0;
}

int onHeaderField(HttpParser* parser, const char *at, std::size_t length)
{
    HttpParserData* data = reinterpret_cast<HttpParserData*>(parser->data);
    data->addCurrentHeader();
    data->currentHead.append(at, length);
    return 0;
}

int onHeaderValue(HttpParser* parser, const char *at, std::size_t length)
{
    HttpParserData* data = reinterpret_cast<HttpParserData*>(parser->data);
    data->gotValue = true;
    data->currentValue.append(at, length);
    return 0;
}

int onBody(HttpParser* parser, const char *at, std::size_t length)
{
    HttpParserData* data = reinterpret_cast<HttpParserData*>(parser->data);
    data->bodyBegin   = at;
    data->bodyEnd     = at + length;
    return 0;
}

HttpParserData::HttpParserData()
    : bodyBegin(nullptr)
    , bodyEnd(nullptr)
    , messageComplete(false)
    , gotValue(false)
{}

void HttpParserData::addCurrentHeader()
{
    if (gotValue)
    {
        headers[currentHead]    = std::move(currentValue);
        gotValue = false;
        currentHead.clear();
        currentValue.clear();
    }
}

HttpScanner::HttpScanner()
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
    parser.data                     = &data;
}

void HttpScanner::scan(char const* buffer, std::size_t size)
{
    std::size_t nparsed = http_parser_execute(&parser, &settings, buffer, size);

    if (parser.upgrade)
    {
        /* handle new protocol */
    }
    else if (nparsed != size)
    {
        /* Handle error. Usually just close the connection. */
    }
}
