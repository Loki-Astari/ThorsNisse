#include "HTTPProtocol.h"
#include <gtest/gtest.h>
#include "Binder.h"
#include "ThorsNisse/NisseService.h"
#include "ThorsNisseSocket/Socket.h"
#include <unistd.h>


using ThorsAnvil::Nisse::ProtocolHTTP::ReadRequestHandler;
using ThorsAnvil::Nisse::ProtocolHTTP::Request;
using ThorsAnvil::Nisse::ProtocolHTTP::Response;
using ThorsAnvil::Nisse::ProtocolHTTP::Method;
using ThorsAnvil::Nisse::ProtocolHTTP::Headers;
using ThorsAnvil::Nisse::ProtocolHTTP::Binder;
using ThorsAnvil::Nisse::ProtocolHTTP::Site;
using ThorsAnvil::Nisse::NisseService;
using ThorsAnvil::Socket::DataSocket;

class ReadRequestHandlerTest: public ReadRequestHandler
{
    ::Method                                      expectedMethod;
    std::string const                             expectedUri;
    std::function<void(::Headers&)>               checkHeaders;
    std::function<void(char const*, char const*)> checkBody;
    public:
        ReadRequestHandlerTest(::NisseService& service, ::DataSocket&& socket, ::Binder const& binder,
                               ::Method expectedMethod,
                               std::string const& expectedUri,
                               std::function<void(::Headers&)> checkHeaders,
                               std::function<void(char const*, char const*)> checkBody)
            : ReadRequestHandler(service, std::move(socket), binder)
            , expectedMethod(expectedMethod)
            , expectedUri(expectedUri)
            , checkHeaders(std::move(checkHeaders))
            , checkBody(std::move(checkBody))
        {}
        void requestComplete(
                     ::DataSocket&&    socket,
                     ::Binder const&   binder,
                     ::Method          method,
                     std::string&&     uri,
                     ::Headers&&       headers,
                     std::vector<char>&& buffer,
                     char const* bodyBegin, char const* bodyEnd)
        {
            EXPECT_EQ(expectedMethod, method);
            EXPECT_EQ(expectedUri,    uri);
            checkHeaders(headers);
            checkBody(bodyBegin, bodyEnd);
        }
};

TEST(HTTPProtocolTest, Construct)
{
    int                 fd[2];
    ASSERT_EQ(0, pipe(fd));

    int     readFD  = fd[0];
    int     writeFD = fd[1];
    ::close(writeFD);

    DataSocket              socket(readFD);
    NisseService            service;
    Binder                  binder;
    ReadRequestHandlerTest  reader(service, std::move(socket), binder, Method::Get, "", [](::Headers&){},[](char const*,char const*){});
}
TEST(HTTPProtocolTest, GetRequest)
{
    int                 fd[2];
    ASSERT_EQ(0, pipe(fd));

    int     readFD  = fd[0];
    int     writeFD = fd[1];

    std::string message = "GET /index.html HTTP/1.1\r\n"
                              "\r\n";
    ::write(writeFD, message.c_str(), message.size());
    close(writeFD);

    DataSocket              socket(readFD);
    NisseService            service;
    Binder                  binder;
    ReadRequestHandlerTest  reader(service, std::move(socket), binder,
                                   Method::Get, "/index.html",
                                   [](::Headers& headers){EXPECT_EQ(headers.begin(), headers.end());},
                                   [](char const* beg,char const* end){EXPECT_EQ(beg, end);});

    reader.eventActivate(readFD, EV_READ);
}
TEST(HTTPProtocolTest, PutRequest)
{
    int                 fd[2];
    ASSERT_EQ(0, pipe(fd));

    int     readFD  = fd[0];
    int     writeFD = fd[1];

    std::string message = "PUT /index.html HTTP/1.1\r\n"
                              "\r\n";
    ::write(writeFD, message.c_str(), message.size());
    close(writeFD);

    DataSocket              socket(readFD);
    NisseService            service;
    Binder                  binder;
    ReadRequestHandlerTest  reader(service, std::move(socket), binder,
                                   Method::Put, "/index.html",
                                   [](::Headers& headers){EXPECT_EQ(headers.begin(), headers.end());},
                                   [](char const* beg,char const* end){EXPECT_EQ(beg, end);});

    reader.eventActivate(readFD, EV_READ);
}
TEST(HTTPProtocolTest, PostRequest)
{
    int                 fd[2];
    ASSERT_EQ(0, pipe(fd));

    int     readFD  = fd[0];
    int     writeFD = fd[1];

    std::string message = "POST /index.html HTTP/1.1\r\n"
                              "\r\n";
    ::write(writeFD, message.c_str(), message.size());
    close(writeFD);

    DataSocket              socket(readFD);
    NisseService            service;
    Binder                  binder;
    ReadRequestHandlerTest  reader(service, std::move(socket), binder,
                                   Method::Post, "/index.html",
                                   [](::Headers& headers){EXPECT_EQ(headers.begin(), headers.end());},
                                   [](char const* beg,char const* end){EXPECT_EQ(beg, end);});

    reader.eventActivate(readFD, EV_READ);
}
TEST(HTTPProtocolTest, DeleteRequest)
{
    int                 fd[2];
    ASSERT_EQ(0, pipe(fd));

    int     readFD  = fd[0];
    int     writeFD = fd[1];

    std::string message = "DELETE /index.html HTTP/1.1\r\n"
                              "\r\n";
    ::write(writeFD, message.c_str(), message.size());
    close(writeFD);

    DataSocket              socket(readFD);
    NisseService            service;
    Binder                  binder;
    ReadRequestHandlerTest  reader(service, std::move(socket), binder,
                                   Method::Delete, "/index.html",
                                   [](::Headers& headers){EXPECT_EQ(headers.begin(), headers.end());},
                                   [](char const* beg,char const* end){EXPECT_EQ(beg, end);});

    reader.eventActivate(readFD, EV_READ);
}
TEST(HTTPProtocolTest, HeadRequest)
{
    int                 fd[2];
    ASSERT_EQ(0, pipe(fd));

    int     readFD  = fd[0];
    int     writeFD = fd[1];

    std::string message = "HEAD /index.html HTTP/1.1\r\n"
                              "\r\n";
    ::write(writeFD, message.c_str(), message.size());
    close(writeFD);

    DataSocket              socket(readFD);
    NisseService            service;
    Binder                  binder;
    ReadRequestHandlerTest  reader(service, std::move(socket), binder,
                                   Method::Head, "/index.html",
                                   [](::Headers& headers){EXPECT_EQ(headers.begin(), headers.end());},
                                   [](char const* beg,char const* end){EXPECT_EQ(beg, end);});

    reader.eventActivate(readFD, EV_READ);
}
TEST(HTTPProtocolTest, CeckHeaders)
{
    int                 fd[2];
    ASSERT_EQ(0, pipe(fd));

    int     readFD  = fd[0];
    int     writeFD = fd[1];

    std::string message = "GET /index.html HTTP/1.1\r\n"
                              "Host: ThorsAnvil.com\r\n"
                              "Content-Type: text/json\r\n"
                              "\r\n";
    ::write(writeFD, message.c_str(), message.size());
    close(writeFD);

    DataSocket              socket(readFD);
    NisseService            service;
    Binder                  binder;
    ReadRequestHandlerTest  reader(service, std::move(socket), binder,
                                   Method::Get, "/index.html",
                                   [](::Headers& headers)
                                    {
                                        EXPECT_EQ(1, headers.getVersions("Host"));
                                        EXPECT_EQ("ThorsAnvil.com", headers.get("Host"));
                                        EXPECT_EQ(1, headers.getVersions("Content-Type"));
                                        EXPECT_EQ("text/json",      headers.get("Content-Type"));
                                    },
                                   [](char const* beg,char const* end){EXPECT_EQ(beg, end);});

    reader.eventActivate(readFD, EV_READ);
}
TEST(HTTPProtocolTest, CeckHeadersMultipleValue)
{
    int                 fd[2];
    ASSERT_EQ(0, pipe(fd));

    int     readFD  = fd[0];
    int     writeFD = fd[1];

    std::string message = "GET /index.html HTTP/1.1\r\n"
                              "Host: ThorsAnvil.com\r\n"
                              "Content-Type: text/json\r\n"
                              "Cookie: Cookie1\r\n"
                              "Cookie:Cookie2\r\n"
                              "Cookie: This is a long cookie with spaces\r\n"
                              "\r\n";
    ::write(writeFD, message.c_str(), message.size());
    close(writeFD);

    DataSocket              socket(readFD);
    NisseService            service;
    Binder                  binder;
    ReadRequestHandlerTest  reader(service, std::move(socket), binder,
                                   Method::Get, "/index.html",
                                   [](::Headers& headers)
                                    {
                                        EXPECT_EQ(1, headers.getVersions("Host"));
                                        EXPECT_EQ("ThorsAnvil.com", headers.get("Host"));
                                        EXPECT_EQ(1, headers.getVersions("Content-Type"));
                                        EXPECT_EQ("text/json",      headers.get("Content-Type"));
                                        EXPECT_EQ(3, headers.getVersions("Cookie"));
                                        EXPECT_EQ("Cookie1",        headers.get("Cookie"));
                                        EXPECT_EQ("Cookie2",        headers.get("Cookie", 1));
                                        EXPECT_EQ("This is a long cookie with spaces",        headers.get("Cookie", 2));
                                    },
                                   [](char const* beg,char const* end){EXPECT_EQ(beg, end);});

    reader.eventActivate(readFD, EV_READ);
}
TEST(HTTPProtocolTest, CheckBody)
{
    int                 fd[2];
    ASSERT_EQ(0, pipe(fd));

    int     readFD  = fd[0];
    int     writeFD = fd[1];

    std::string message = "PUT /index.html HTTP/1.1\r\n"
                              "Content-Length: 21\r\n"
                              "\r\n"
                              "This is a body object";
    ::write(writeFD, message.c_str(), message.size());
    close(writeFD);

    DataSocket              socket(readFD);
    NisseService            service;
    Binder                  binder;
    ReadRequestHandlerTest  reader(service, std::move(socket), binder,
                                   Method::Put, "/index.html",
                                   [](::Headers& headers){EXPECT_EQ(++(headers.begin()), headers.end());},
                                   [](char const* beg,char const* end)
                                    {
                                        EXPECT_EQ("This is a body object", std::string(beg, end));
                                    });

    reader.eventActivate(readFD, EV_READ);
}

/*
TEST(HTTPProtocolTest, ConstructWriter)
{
    int                 fd[2];
    ASSERT_EQ(0, pipe(fd));

    int     readFD  = fd[0];
    int     writeFD = fd[1];

    DataSocket              socket(writeFD);
    NisseService            service;
    Binder                  binder;
	std::string				uri = "thorsanvil.com/index.html";
	Headers					headers;
	std::vector<char>		buffer;
    WriteResponseHandler  writer(service, std::move(socket), binder, Method::Get, std::move(uri), std::move(headers), std::move(buffer), nullptr, nullptr);
}
TEST(HTTPProtocolTest, WriterProcesses)
{
    int                 fd[2];
    ASSERT_EQ(0, pipe(fd));

    int     readFD  = fd[0];
    int     writeFD = fd[1];

    bool        hitThorsAnvil = false;;
    Site        thorsAnvil;
    thorsAnvil.get("/index.html", [&hitThorsAnvil](Request&, Response&) {hitThorsAnvil = true;});

    bool        hitDefault = false;;
    Site        defaultSite;
    defaultSite.get("/index.html", [&hitDefault](Request&, Response&) {hitDefault = true;});

    Binder                  binder;
    binder.addSite("", std::move(defaultSite));
    binder.addSite("ThorsAnvil.com", std::move(thorsAnvil));

	Headers					headers;
    headers["Host"] = "ThorsAnvil.com";

    DataSocket              socket(writeFD);
    NisseService            service;
	std::string				uri = "/index.html";
	std::vector<char>		buffer;
    WriteResponseHandler  writer(service, std::move(socket), binder, Method::Get, std::move(uri), std::move(headers), std::move(buffer), nullptr, nullptr);
    writer.eventActivate(writeFD, EV_READ);

    ASSERT_FALSE(hitDefault);
    ASSERT_TRUE(hitThorsAnvil);
}
*/
