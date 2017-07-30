#include "Binder.h"
#include <gtest/gtest.h>
#include "Types.h"
#include "ThorsNisseSocket/Socket.h"
#include <vector>
#include <sys/stat.h> 
#include <fcntl.h>

using ThorsAnvil::Nisse::ProtocolHTTP::Site;
using ThorsAnvil::Nisse::ProtocolHTTP::Binder;

using ThorsAnvil::Nisse::ProtocolHTTP::Request;
using ThorsAnvil::Nisse::ProtocolHTTP::Response;
using ThorsAnvil::Nisse::ProtocolHTTP::Method;
using ThorsAnvil::Nisse::ProtocolHTTP::Yield;
using ThorsAnvil::Nisse::ProtocolHTTP::URI;
using ThorsAnvil::Nisse::ProtocolHTTP::Headers;
using ThorsAnvil::Socket::DataSocket;

void callMethodExpectFail(Method method, Site& site, std::string const& host, std::string&& path)
{
    auto find = site.find(method, "/The/Path/Line");
    ASSERT_FALSE(find.first);
}
void callMethod(Method method, Site& site, std::string const& host, std::string&& path)
{
    auto find = site.find(method, "/The/Path/Line");
    ASSERT_TRUE(find.first);

    int                 fd = open("test/data/bind", O_RDONLY);
    DataSocket          stream(fd);
    Yield               yield;
    URI                 uri(host, std::move(path));
    Headers             headers;
    std::vector<char>   data;
    Request             request(stream, yield, Method::Get, std::move(uri), std::move(headers), std::move(data), nullptr, nullptr);
    Response            response(stream, yield);

    find.second(request, response);
}

TEST(BinderTest, ConstructBuild)
{
    Site        site;
}
TEST(BinderTest, AddAGetMethod)
{
    Site        site;
    bool        called = false;
    site.get("/The/Path/Line", [&called](Request& /*request*/, Response& /*response*/) {called = true;});

    callMethodExpectFail(Method::Put,    site, "ThorsAnvil.com", "/The/Path/Line");
    ASSERT_FALSE(called);
    callMethodExpectFail(Method::Post,   site, "ThorsAnvil.com", "/The/Path/Line");
    ASSERT_FALSE(called);
    callMethodExpectFail(Method::Delete, site, "ThorsAnvil.com", "/The/Path/Line");
    ASSERT_FALSE(called);

    callMethod(Method::Get, site, "ThorsAnvil.com", "/The/Path/Line");
    ASSERT_TRUE(called);
    called = false;
    callMethod(Method::Head,   site, "ThorsAnvil.com", "/The/Path/Line");
    ASSERT_TRUE(called);
}
TEST(BinderTest, AddAPutMethod)
{
    Site        site;
    bool        called = false;
    site.put("/The/Path/Line", [&called](Request& /*request*/, Response& /*response*/) {called = true;});

    callMethodExpectFail(Method::Get,    site, "ThorsAnvil.com", "/The/Path/Line");
    ASSERT_FALSE(called);
    callMethodExpectFail(Method::Post,   site, "ThorsAnvil.com", "/The/Path/Line");
    ASSERT_FALSE(called);
    callMethodExpectFail(Method::Delete, site, "ThorsAnvil.com", "/The/Path/Line");
    ASSERT_FALSE(called);
    callMethodExpectFail(Method::Head,   site, "ThorsAnvil.com", "/The/Path/Line");
    ASSERT_FALSE(called);

    callMethod(Method::Put, site, "ThorsAnvil.com", "/The/Path/Line");
    ASSERT_TRUE(called);
}
TEST(BinderTest, AddAPostMethod)
{
    Site        site;
    bool        called = false;
    site.post("/The/Path/Line", [&called](Request& /*request*/, Response& /*response*/) {called = true;});

    callMethodExpectFail(Method::Get,    site, "ThorsAnvil.com", "/The/Path/Line");
    ASSERT_FALSE(called);
    callMethodExpectFail(Method::Put,    site, "ThorsAnvil.com", "/The/Path/Line");
    ASSERT_FALSE(called);
    callMethodExpectFail(Method::Delete, site, "ThorsAnvil.com", "/The/Path/Line");
    ASSERT_FALSE(called);
    callMethodExpectFail(Method::Head,   site, "ThorsAnvil.com", "/The/Path/Line");
    ASSERT_FALSE(called);

    callMethod(Method::Post, site, "ThorsAnvil.com", "/The/Path/Line");
    ASSERT_TRUE(called);
}
TEST(BinderTest, AddADeleteMethod)
{
    Site        site;
    bool        called = false;
    site.del("/The/Path/Line", [&called](Request& /*request*/, Response& /*response*/) {called = true;});

    callMethodExpectFail(Method::Get,    site, "ThorsAnvil.com", "/The/Path/Line");
    ASSERT_FALSE(called);
    callMethodExpectFail(Method::Put,    site, "ThorsAnvil.com", "/The/Path/Line");
    ASSERT_FALSE(called);
    callMethodExpectFail(Method::Post,   site, "ThorsAnvil.com", "/The/Path/Line");
    ASSERT_FALSE(called);
    callMethodExpectFail(Method::Head,   site, "ThorsAnvil.com", "/The/Path/Line");
    ASSERT_FALSE(called);

    callMethod(Method::Delete, site, "ThorsAnvil.com", "/The/Path/Line");
    ASSERT_TRUE(called);
}

TEST(BinderTest, Construct)
{
    Binder  binder;
}
TEST(BinderTest, AddSites)
{
    bool calledSite1 = false;
    bool calledSite2 = false;
    bool calledSite3 = false;
    bool missed      = false;

    Site    site1;
    site1.get("/pathSpecific", [&calledSite1](Request& /*request*/, Response& /*response*/) {calledSite1 = true;});

    Site    site2;
    site2.get("/pathSpecific", [&calledSite2](Request& /*request*/, Response& /*response*/) {calledSite2 = true;});

    Site    site3;
    site3.get("/pathDefault",  [&calledSite3](Request& /*request*/, Response& /*response*/) {calledSite3 = true;});


    Binder  binder;
    binder.setCustome404Action([&missed](Request& /*request*/, Response& /*response*/) {missed = true;});
    binder.addSite("ThorsAnvil.com", std::move(site1));
    binder.addSite("LokiAstari.com", std::move(site2));
    binder.addSite("", std::move(site3));


    int                 fd = open("test/data/bind", O_RDONLY);
    DataSocket          stream(fd);
    Yield               yield;
    URI                 uri("Ignored", "Ignored");
    Headers             headers;
    std::vector<char>   data;
    Request             request(stream, yield, Method::Get, std::move(uri), std::move(headers), std::move(data), nullptr, nullptr);
    Response            response(stream, yield);

    auto action = binder.find(Method::Get, "ThorsAnvil.com", "/pathSpecific");
    action(request, response);
    ASSERT_TRUE(calledSite1);

    action = binder.find(Method::Get, "ThorsAnvil.com", "/pathSpecific-Missing");
    action(request, response);
    ASSERT_TRUE(missed);

    ASSERT_FALSE(calledSite2);
    ASSERT_FALSE(calledSite3);

    calledSite1 = false;
    missed      = false;

    action = binder.find(Method::Get, "LokiAstari.com", "/pathSpecific");
    action(request, response);
    ASSERT_TRUE(calledSite2);

    action = binder.find(Method::Get, "LokiAstari.com", "/pathSpecific-Missing");
    action(request, response);
    ASSERT_TRUE(missed);

    ASSERT_FALSE(calledSite1);
    ASSERT_FALSE(calledSite3);

    calledSite2 = false;
    missed      = false;

    action = binder.find(Method::Get, "ThorsAnvil.com", "/pathDefault");
    action(request, response);
    ASSERT_TRUE(calledSite3);
}

/*
class Binder
{
    private:
        std::map<std::string, Site> siteMap;
    public:
        void addSite(std::string const& host, Site&& site);
        Action& find(Method method, std::string const& host, std::string const& path) const;
};
*/
