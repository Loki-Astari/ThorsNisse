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
    private:
        Container data;
    public:
        typedef ConstIterator const_iterator;

        ValueStore& operator[](std::string const& key)    {return data[key];}

        std::size_t        getVersions(std::string const& key) const
        {
            auto find = data.find(key);
            return (find == data.end()) ? 0 : find->second.size();
        }

        std::string const& get(std::string const& key, std::size_t version = 0) const
        {
            static const std::string empty;

            auto find = data.find(key);
            if (find == data.end())
            {
                return empty;
            }
            return version >= find->second.size() ? empty : find->second[version];
        }

        ConstIterator begin() const {return std::cbegin(data);}
        ConstIterator end()   const {return std::cend(data);}
};

class URI
{
    public:
        const std::string original;
        const std::string normalized;

        const std::string schema;
        const std::string host;
        const std::string path;
        const std::string query;
        const std::string fragment;
        const short       port;

        const Headers     queryParam;

        URI(std::string&& original)
            : original(original)
            , port(80)
        {}
};

class Request
{
    public:
        const Method            method;
        const URI               uri;
        const Headers           headers;
        Socket::ISocketStream   body;

        Request(Socket::DataSocket& stream, Yield& yield, Method method, URI&& uri, Headers&& headers, std::vector<char>&& data, char const* beg, char const* end)
            : method(method)
            , uri(uri)
            , headers(std::move(headers))
            , body(stream, [&yield](){yield();}, [](){}, std::move(data), beg, end)
        {}
};

class Response
{
    public:
        short                   resultCode;
        std::string             resultMessage;
        Headers                 headers;
        Socket::OSocketStream   body;

        Response(Socket::DataSocket& stream, Yield& yield, short resultCode = 200, std::string const& resultMessage = "OK")
            : resultCode(resultCode)
            , resultMessage(resultMessage)
            , body(stream, [&yield](){yield();}, [&parent = *this](){})
        {}
};

        }
    }
}

#endif
