#include "Binder.h"
#include <gtest/gtest.h>
#include "Types.h"
#include "ThorsNisseCoreSocket/Socket.h"
#include <vector>

using ThorsAnvil::Nisse::Protocol::HTTP::Site;
using ThorsAnvil::Nisse::Protocol::HTTP::Binder;

using ThorsAnvil::Nisse::Protocol::HTTP::Request;
using ThorsAnvil::Nisse::Protocol::HTTP::Response;
using ThorsAnvil::Nisse::Protocol::HTTP::Method;
using ThorsAnvil::Nisse::Protocol::HTTP::Yield;
using ThorsAnvil::Nisse::Protocol::HTTP::URI;
using ThorsAnvil::Nisse::Protocol::HTTP::Headers;
using ThorsAnvil::Nisse::Core::Socket::DataSocket;

void callMethodExpectFail(Method method, Site& site, std::string const& /*host*/, std::string&& /*path*/)
{
    auto find = site.find(method, "/The/Path/Line");
    ASSERT_FALSE(find.first);
}
void callMethod(Method method, Site& site, std::string const& host, std::string&& path)
{
    auto find = site.find(method, "/The/Path/Line");
    ASSERT_TRUE(find.first);

    URI                 uri(host, std::move(path));
    Headers             headers;
    std::stringstream   body;
    Request             request(Method::Get, std::move(uri), headers, body);
    Response            response(body);

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
TEST(BinderTest, TestSite404)
{
    Site        site;
    auto action = site.find(Method::Get, "/Miss");
    ASSERT_FALSE(action.first);


    URI                 uri("THorsAnvil.com", "/Miss");
    Headers             headers;
    std::stringstream   body;
    Request             request(Method::Get, std::move(uri), headers, body);
    Response            response(body);
    action.second(request, response);
    ASSERT_EQ("", body.str());
}
TEST(BinderTest, TestBinder404)
{
    Binder              binder;
    auto action = binder.find(Method::Get, "ThorsAnvil.com", "/Miss");


    URI                 uri("THorsAnvil.com", "/Miss");
    Headers             headers;
    std::stringstream   body;
    Request             request(Method::Get, std::move(uri), headers, body);
    Response            response(body);
    action(request, response);
    ASSERT_EQ("<html><body><h1>Not Found</h1></body></html>", body.str());

    ASSERT_EQ(404, response.resultCode);
    ASSERT_EQ("Not Found", response.resultMessage);
    //ASSERT_EQ(">Date String<", response.headers.get("Date"));
    ASSERT_EQ(ThorsAnvil::Nisse::Protocol::HTTP::ServerName,       response.headers.get(ThorsAnvil::Nisse::Protocol::HTTP::Head_Server));
    ASSERT_EQ(ThorsAnvil::Nisse::Protocol::HTTP::Connection_Closed,response.headers.get(ThorsAnvil::Nisse::Protocol::HTTP::Head_Connection));
    ASSERT_EQ("44",             response.headers.get(ThorsAnvil::Nisse::Protocol::HTTP::Head_ContentLen));
    ASSERT_EQ("text/html",      response.headers.get(ThorsAnvil::Nisse::Protocol::HTTP::Head_ContentType));
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
TEST(BinderTest, AddAllMethod)
{
    Site        site;
    bool        called = false;
    site.all("/The/Path/Line", [&called](Request& /*request*/, Response& /*response*/) {called = true;});

    callMethod(Method::Get,    site, "ThorsAnvil.com", "/The/Path/Line");
    ASSERT_TRUE(called);
    called = false;
    callMethod(Method::Put,    site, "ThorsAnvil.com", "/The/Path/Line");
    ASSERT_TRUE(called);
    called = false;
    callMethod(Method::Post,   site, "ThorsAnvil.com", "/The/Path/Line");
    ASSERT_TRUE(called);
    called = false;
    callMethod(Method::Head,   site, "ThorsAnvil.com", "/The/Path/Line");
    ASSERT_TRUE(called);
    called = false;
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
    binder.addSite("ThorsAnvil.com", "", std::move(site1));
    binder.addSite("LokiAstari.com", "", std::move(site2));
    binder.addSite("", "", std::move(site3));


    URI                 uri("Ignored", "Ignored");
    Headers             headers;
    std::stringstream   body;
    Request             request(Method::Get, std::move(uri), headers, body);
    Response            response(body);

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
