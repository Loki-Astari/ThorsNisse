#ifndef THORSANVIL_NISSE_PROTOCOL_HTTP_HTTP_SCANNER_H
#define THORSANVIL_NISSE_PROTOCOL_HTTP_HTTP_SCANNER_H

#include "http_parser.h"
#include "Types.h"
#include <string>

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace Protocol
        {
            namespace HTTP
            {

using HttpParser            = http_parser;
using HttpParserSettings    = http_parser_settings;

// @class
struct HttpParserData
{
    HttpParserData();
    void addCurrentHeader();

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

// @class
class HttpScanner
{
    HttpParserSettings      settings;
    HttpParser              parser;

    public:
        HttpParserData          data;

        HttpScanner();
        void scan(char const* buffer, std::size_t size);
};

            }
        }
    }
}

#endif
