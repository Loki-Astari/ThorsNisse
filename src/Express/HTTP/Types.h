#ifndef THORSANVIL_NISSE_PROTOCOL_HTTP_TYPES_H
#define THORSANVIL_NISSE_PROTOCOL_HTTP_TYPES_H

#include "ThorsNisseCoreSocket/Socket.h"
#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include <map>
#include <ctime>

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace Protocol
        {
            namespace HTTP
            {

static std::string const Head_Server        = "Server";
static std::string const Head_Date          = "Date";
static std::string const Head_ContentLen    = "Content-Length";
static std::string const Head_ContentType   = "Content-Type";
static std::string const Head_Connection    = "Connection";

static std::string const Resp_200           = "OK";
static std::string const Resp_404           = "Not Found";

static std::string const Connection_Closed  = "Closed";

static std::string const ServerName         = "Nisse V1.0";

enum class Method {Get, Put, Post, Delete, Head};

inline char const* getTimeString()
{
    using TimeT = std::time_t;
    using TimeI = std::tm;

    TimeT   rawtime;
    TimeI*  timeinfo;

    ::time(&rawtime);
    timeinfo = ::localtime(&rawtime);

    return ::asctime(timeinfo);
}


// @class
class Headers
{
    using ValueStore    = std::vector<std::string>;
    using Container     = std::map<std::string, ValueStore>;
    using ConstIterator = Container::const_iterator;
    Container data;

    public:
        typedef ConstIterator const_iterator;
        class Inserter
        {
            ValueStore&  valueStore;
            public:
                Inserter(ValueStore& valueStore);
                void operator=(std::string&& value);
                void operator=(std::string const& value);
        };

        ConstIterator begin() const {return std::cbegin(data);}
        ConstIterator end()   const {return std::cend(data);}
        Inserter operator[](std::string const& key)    {return data[key];}

        std::size_t        getVersions(std::string const& key) const;
        std::string const& get(std::string const& key, std::size_t version = 0) const;
};

// @class
class URI
{
    public:
        std::string original;
        std::string normalized;

        std::string schema;
        std::string host;
        std::string path;
        std::string query;
        std::string fragment;
        short       port;

        Headers     queryParam;

        URI(std::string const& hostAndPort, std::string&& pathAndQuery);
    private:
        std::string::size_type findSection(std::string const& s, char value, std::string::size_type pos);
};

// @class
class Request
{
    public:
        const Method            method;
        const URI               uri;
        const Headers&          headers;
        std::istream&           body;

        Request(Method method,
                URI&& uri,
                Headers& headers,
                std::istream& body);
};

class ReadRequestHandler;
// @class
class Response
{
    using DataSocket = ThorsAnvil::Nisse::Core::Socket::DataSocket;
    private:
        ReadRequestHandler*    flusher;
        DataSocket*             socket;
        bool                    headerWritten;
    public:
        short                   resultCode;
        std::string             resultMessage;
        Headers                 headers;
        std::ostream&           body;

        Response(std::ostream& body);
        Response(ReadRequestHandler& flusher,
                 DataSocket& socket,
                 std::ostream& body,
                 short resultCode = 200,
                 std::string const& resultMessage = "OK");
        ~Response();
        void flushing(bool allDone = false);
};

            }
        }
    }
}

#endif
