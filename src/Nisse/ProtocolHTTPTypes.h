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
    private:
        Container data;
    public:
        typedef ConstIterator const_iterator;
        class Inserter
        {
            private:
                ValueStore&  valueStore;
            public:
                Inserter(ValueStore& valueStore)
                    : valueStore(valueStore)
                {}

                void operator=(std::string&& value)
                {
                    valueStore.emplace_back(std::move(value));
                    auto& item = valueStore.back();
                    std::transform(std::begin(item), std::end(item), std::begin(item), [](char x){return x == '\n' ? ' ' : x;});
                }
        };

        Inserter operator[](std::string const& key)    {return data[key];}

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
        std::string original;
        std::string normalized;

        std::string schema;
        std::string host;
        std::string path;
        std::string query;
        std::string fragment;
        short       port;

        Headers     queryParam;

        URI(std::string const& hostAndPort, std::string&& pathAndQuery)
            : port(80)
        {
            auto find = hostAndPort.find(':');
            if (find == std::string::npos)
            {
                host    = hostAndPort;
            }
            else
            {
                host    = hostAndPort.substr(0, find);
                port    = atoi(hostAndPort.substr(find + 1).c_str());
            }

            auto nextSection = pathAndQuery.find_first_of("?&#", 0, 3);
            if (nextSection == std::string::npos)
            {
                path    = std::move(pathAndQuery);
            }
            else
            {
                path    = pathAndQuery.substr(0, nextSection);
                if (pathAndQuery[nextSection] != '#')
                {
                    auto querySection = nextSection;
                    nextSection = findSection(pathAndQuery, '#', querySection);
                    query    = pathAndQuery.substr(querySection, (nextSection - querySection));
                }
                fragment = pathAndQuery.substr(nextSection);
            }

            if (!query.empty())
            {
                std::string::size_type lastPart = 0;
                for(auto nextPart = findSection(query, '&', 0); nextPart != lastPart; lastPart = nextPart, nextPart = findSection(query, '&', lastPart))
                {
                    auto split = findSection(query, '=', lastPart);
                    split = split > nextPart ? nextPart - 1 : split;
                    queryParam[query.substr(lastPart + 1, (split - lastPart - 1))] = query.substr(split, (nextPart - split));
                }
            }
        }
    private:
        std::string::size_type findSection(std::string const& s, char value, std::string::size_type pos)
        {
            if (pos == s.size())
            {
                return pos;
            }
            auto find = s.find(value, pos + 1);
            return find == std::string::npos ? s.size() : find;
        }
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
            , body(stream, [&yield](){yield();}, [](){}, false, std::move(data), beg, end)
        {}
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

        Response(Socket::DataSocket& stream, Yield& yield, short resultCode = 200, std::string const& resultMessage = "OK")
            : headerWritten(false)
            , stream(stream)
            , yield(yield)
            , resultCode(resultCode)
            , resultMessage(resultMessage)
            , body(stream, [&yield](){yield();}, [&parent = *this](){parent.flushing();})
        {}
        void flushing()
        {
            if (!headerWritten)
            {
                headerWritten = true;
                Socket::OSocketStream headerStream(stream, [&yield = this->yield](){yield();}, [](){}, false);

                headerStream << "HTTP/1.1 " << resultCode << " " << resultMessage << "\r\n";
                for (auto const& header: headers)
                {
                    for (auto const& value: header.second)
                    {
                        headerStream << header.first << ": " << value << "\r\n";
                    }
                }
                headerStream << "\r\n";
            }
        }
};

        }
    }
}

#endif
