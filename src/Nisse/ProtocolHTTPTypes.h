#ifndef THORSANVIL_NISSE_PROTOCOLHTTP_TYPES_H
#define THORSANVIL_NISSE_PROTOCOLHTTP_TYPES_H

#include "ThorsNisseSocket/Socket.h"
#include "ThorsNisseSocket/SocketStream.h"
#include <boost/coroutine/asymmetric_coroutine.hpp>
#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace ProtocolHTTP
        {

using CoRoutine = boost::coroutines::asymmetric_coroutine<void>::pull_type;
using Yield     = boost::coroutines::asymmetric_coroutine<void>::push_type;

enum class Method {Get, Put, Post, Delete, Head};

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
        };

        ConstIterator begin() const {return std::cbegin(data);}
        ConstIterator end()   const {return std::cend(data);}
        Inserter operator[](std::string const& key)    {return data[key];}

        std::size_t        getVersions(std::string const& key) const;
        std::string const& get(std::string const& key, std::size_t version = 0) const;
};

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

class Request
{
    public:
        const Method            method;
        const URI               uri;
        const Headers           headers;
        Socket::ISocketStream   body;

        Request(Socket::DataSocket& stream,
                Yield& yield,
                Method method,
                URI&& uri,
                Headers&& headers,
                std::vector<char>&& data, char const* beg, char const* end);
};

class Response
{
    private:
        bool                    headerWritten;
        Socket::DataSocket&     stream;
        Yield&                  yield;
    public:
        short                   resultCode;
        std::string             resultMessage;
        Headers                 headers;
        Socket::OSocketStream   body;

        Response(Socket::DataSocket& stream,
                 Yield& yield,
                 short resultCode = 200,
                 std::string const& resultMessage = "OK");
        void flushing();
};

        }
    }
}

#endif
