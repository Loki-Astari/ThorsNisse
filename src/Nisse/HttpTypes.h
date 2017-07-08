#ifndef THORSANVIL_NISSE_HTTP_TYPES_H
#define THORSANVIL_NISSE_HTTP_TYPES_H

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

using CoRoutine = boost::coroutines::asymmetric_coroutine<void>::pull_type;
using Yield     = boost::coroutines::asymmetric_coroutine<void>::push_type;

enum class HttpMethod {Get, Put, Post, Delete, Head};

class Headers
{
    using ValueStore    = std::vector<std::string>;
    using Container     = std::map<std::string, ValueStore>;
    using ConstIterator = Container::const_iterator;
    private:
        std::map<std::string, std::vector<std::string>> data;
    public:
        typedef ConstIterator const_iterator;

        std::vector<std::string>& operator[](std::string const& key)    {return data[key];}

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

        ConstIterator begin() const {return std::begin(data);}
        ConstIterator end()   const {return std::end(data);}
};

class ISocketStream: public std::istream
{
    Yield&      yield;
    public:
        ISocketStream(Yield& yield, std::vector<char>&&, char const*, char const*)
            : std::istream(nullptr)
            , yield(yield)
        {}
        ISocketStream(ISocketStream&& move)
            : std::istream(nullptr)
            , yield(move.yield)
        {}
        /*
        ISocketStream& operator=(ISocketStream&&)
        {
            return *this;
        }
        */
};

class OSocketStream: public std::ostream
{
    Yield&      yield;
    public:
        OSocketStream(Yield& yield)
            : std::ostream(nullptr)
            , yield(yield)
        {}
        OSocketStream(OSocketStream&& move)
            : std::ostream(nullptr)
            , yield(move.yield)
        {}
        /*
        OSocketStream& operator=(OSocketStream&&)
        {
            return *this;
        }
        */
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

class HTTPRequest
{
    public:
        const HttpMethod    method;
        const URI           uri;
        const Headers       headers;
        ISocketStream       body;

        HTTPRequest(Yield& yield, HttpMethod method, URI&& uri, Headers&& headers, std::vector<char>&& data, char const* beg, char const* end)
            : method(method)
            , uri(uri)
            , headers(std::move(headers))
            , body(yield, std::move(data), beg, end)
        {}
};

class HTTPResponse
{
    public:
        short           resultCode;
        std::string     resultMessage;
        Headers         headers;
        OSocketStream   body;

        HTTPResponse(Yield& yield, short resultCode = 200, std::string const& resultMessage = "OK")
            : resultCode(resultCode)
            , resultMessage(resultMessage)
            , body(yield)
        {}
};

    }
}

#endif
