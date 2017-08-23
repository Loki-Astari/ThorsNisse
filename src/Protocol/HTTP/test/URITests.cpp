#include "Types.h"
#include <gtest/gtest.h>

// ThorsAnvil.com
// ThorsAnvil.com:8088
// ThorsAnvil.com /A/Path
// ThorsAnvil.com /A/Path?Query=2

/*
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
*/

using ThorsAnvil::Nisse::Protocol::HTTP::URI;

TEST(URITests, ConstructHostOnly)
{
    URI     uri("ThorsAnvil.com", "");
    ASSERT_EQ("ThorsAnvil.com", uri.host);
    ASSERT_EQ(80,               uri.port);
}
TEST(URITests, ConstructHostAndPort)
{
    URI     uri("ThorsAnvil.com:8088", "");
    ASSERT_EQ("ThorsAnvil.com", uri.host);
    ASSERT_EQ(8088,             uri.port);
}
TEST(URITests, ConstructorPathOnly)
{
    URI     uri("ThorsAnvil.com", "/A/Path");
    ASSERT_EQ("/A/Path",        uri.path);
    ASSERT_EQ("",               uri.query);
    ASSERT_EQ("",               uri.fragment);
}
TEST(URITests, ConstructorPathQuery)
{
    URI     uri("ThorsAnvil.com", "/A/Path?plop=2");
    ASSERT_EQ("/A/Path",        uri.path);
    ASSERT_EQ("?plop=2",        uri.query);
    ASSERT_EQ("",               uri.fragment);
}
TEST(URITests, ConstructorPathQueryMultipleParam)
{
    URI     uri("ThorsAnvil.com", "/A/Path?plop=2&val=3");
    ASSERT_EQ("/A/Path",        uri.path);
    ASSERT_EQ("?plop=2&val=3",  uri.query);
    ASSERT_EQ("",               uri.fragment);
}
TEST(URITests, ConstructorPathQueryMultipleParamCommonMissingQMark)
{
    URI     uri("ThorsAnvil.com", "/A/Path&plop=2&val=3");
    ASSERT_EQ("/A/Path",        uri.path);
    ASSERT_EQ("?plop=2&val=3",  uri.query);
    ASSERT_EQ("",               uri.fragment);
}
TEST(URITests, ConstructorPathFragment)
{
    URI     uri("ThorsAnvil.com", "/A/Path#Frag");
    ASSERT_EQ("/A/Path",        uri.path);
    ASSERT_EQ("",               uri.query);
    ASSERT_EQ("#Frag",          uri.fragment);
}
TEST(URITests, ConstructorQueryFragment)
{
    URI     uri("ThorsAnvil.com", "?glob=13#Frag2");
    ASSERT_EQ("",               uri.path);
    ASSERT_EQ("?glob=13",       uri.query);
    ASSERT_EQ("#Frag2",         uri.fragment);
}
TEST(URITests, ConstructorPathQueryFragment)
{
    URI     uri("ThorsAnvil.com", "/The/Path?glob=13#Frag2");
    ASSERT_EQ("/The/Path",      uri.path);
    ASSERT_EQ("?glob=13",       uri.query);
    ASSERT_EQ("#Frag2",         uri.fragment);
}

