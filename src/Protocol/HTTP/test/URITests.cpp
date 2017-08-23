#include "Types.h"
#include <gtest/gtest.h>

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

