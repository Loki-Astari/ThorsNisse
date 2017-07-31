#include "Types.h"

/*
#include "ThorsNisseSocket/Socket.h"
#include "ThorsNisseSocket/SocketStream.h"
#include <boost/coroutine/asymmetric_coroutine.hpp>
#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
*/

using namespace ThorsAnvil::Nisse::ProtocolHTTP;

Headers::Inserter::Inserter(ValueStore& valueStore)
    : valueStore(valueStore)
{}

void Headers::Inserter::operator=(std::string&& value)
{
    valueStore.emplace_back(std::move(value));
    auto& item = valueStore.back();
    std::transform(std::begin(item), std::end(item), std::begin(item), [](char x){return x == '\n' ? ' ' : x;});
}

std::size_t Headers::getVersions(std::string const& key) const
{
    auto find = data.find(key);
    return (find == data.end()) ? 0 : find->second.size();
}

std::string const& Headers::get(std::string const& key, std::size_t version) const
{
    static const std::string empty;

    auto find = data.find(key);
    if (find == data.end())
    {
        return empty;
    }
    return version >= find->second.size() ? empty : find->second[version];
}

URI::URI(std::string const& hostAndPort, std::string&& pathAndQuery)
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
        for (auto nextPart = findSection(query, '&', 0); nextPart != lastPart; lastPart = nextPart, nextPart = findSection(query, '&', lastPart))
        {
            auto split = findSection(query, '=', lastPart);
            split = split > nextPart ? nextPart - 1 : split;
            queryParam[query.substr(lastPart + 1, (split - lastPart - 1))] = query.substr(split, (nextPart - split));
        }
    }
}

std::string::size_type URI::findSection(std::string const& s, char value, std::string::size_type pos)
{
    if (pos == s.size())
    {
        return pos;
    }
    auto find = s.find(value, pos + 1);
    return find == std::string::npos ? s.size() : find;
}

Request::Request(Method method,
                 URI&& uri,
                 Headers& headers,
                 std::istream& body)
    : method(method)
    , uri(std::move(uri))
    , headers(headers)
    , body(body)
{}

Response::Response(Socket::DataSocket& stream,
                   Yield& yield,
                   short resultCode,
                   std::string const& resultMessage)
    : headerWritten(false)
    , stream(stream)
    , yield(yield)
    , resultCode(resultCode)
    , resultMessage(resultMessage)
    , body(stream, [&yield](){yield();}, [&parent = *this](){parent.flushing();})
{}

void Response::flushing()
{
    if (!headerWritten)
    {
        headerWritten = true;
        Socket::OSocketStream headerStream(stream, [&yield = this->yield](){yield();}, [](){});

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
