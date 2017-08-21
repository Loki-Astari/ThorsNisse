#include "HTTPProtocol.h"
#include <gtest/gtest.h>
#include "Binder.h"
#include "ThorsNisseCoreService/Server.h"
#include "ThorsNisseCoreSocket/Socket.h"
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


using ThorsAnvil::Nisse::Protocol::HTTP::ReadRequestHandler;
using ThorsAnvil::Nisse::Protocol::HTTP::Request;
using ThorsAnvil::Nisse::Protocol::HTTP::Response;
using ThorsAnvil::Nisse::Protocol::HTTP::Method;
using ThorsAnvil::Nisse::Protocol::HTTP::Headers;
using ThorsAnvil::Nisse::Protocol::HTTP::Binder;
using ThorsAnvil::Nisse::Protocol::HTTP::Site;
using ThorsAnvil::Nisse::Core::Service::Server;
using ThorsAnvil::Nisse::Core::Socket::DataSocket;

using namespace std;
 
// A new one of these is created for each test
class HTTPProtocolTest : public testing::Test
{
    public:
        virtual void SetUp()
        {
            Server::ignore("epoll");
        }

        virtual void TearDown()
        {
            Server::ignore();
        }
};
 
TEST_F(HTTPProtocolTest, Construct)
{
    unlink("XX");
    int         readFD = ::open("XX", O_RDWR | O_CREAT);

    DataSocket              socket(readFD);
    Server                  service;
    Binder                  binder;
    ReadRequestHandler      reader(service, std::move(socket), binder);
    unlink("XX");
}
TEST_F(HTTPProtocolTest, GetRequest)
{
    unlink("XX");
    int         readFD = ::open("XX", O_RDWR | O_CREAT);

    std::string message = "GET /index.html HTTP/1.1\r\n"
                              "\r\n";
    ::write(readFD, message.c_str(), message.size());

    DataSocket              socket(readFD);
    Server                  service;
    Binder                  binder;
    ReadRequestHandler      reader(service, std::move(socket), binder);

    reader.eventActivate(readFD, EV_READ);
    unlink("XX");
}
TEST_F(HTTPProtocolTest, PutRequest)
{
    unlink("XX");
    int         readFD = ::open("XX", O_RDWR | O_CREAT);

    std::string message = "PUT /index.html HTTP/1.1\r\n"
                              "\r\n";
    ::write(readFD, message.c_str(), message.size());

    DataSocket              socket(readFD);
    Server                  service;
    Binder                  binder;
    ReadRequestHandler      reader(service, std::move(socket), binder);

    reader.eventActivate(readFD, EV_READ);
    unlink("XX");
}
TEST_F(HTTPProtocolTest, PostRequest)
{
    unlink("XX");
    int         readFD = ::open("XX", O_RDWR | O_CREAT);

    std::string message = "POST /index.html HTTP/1.1\r\n"
                              "\r\n";
    ::write(readFD, message.c_str(), message.size());

    DataSocket              socket(readFD);
    Server                  service;
    Binder                  binder;
    ReadRequestHandler      reader(service, std::move(socket), binder);

    reader.eventActivate(readFD, EV_READ);
    unlink("XX");
}
TEST_F(HTTPProtocolTest, DeleteRequest)
{
    unlink("XX");
    int         readFD = ::open("XX", O_RDWR | O_CREAT);

    std::string message = "DELETE /index.html HTTP/1.1\r\n"
                              "\r\n";
    ::write(readFD, message.c_str(), message.size());

    DataSocket              socket(readFD);
    Server                  service;
    Binder                  binder;
    ReadRequestHandler      reader(service, std::move(socket), binder);

    reader.eventActivate(readFD, EV_READ);
    unlink("XX");
}
TEST_F(HTTPProtocolTest, HeadRequest)
{
    unlink("XX");
    int         readFD = ::open("XX", O_RDWR | O_CREAT);

    std::string message = "HEAD /index.html HTTP/1.1\r\n"
                              "\r\n";
    ::write(readFD, message.c_str(), message.size());

    DataSocket              socket(readFD);
    Server                  service;
    Binder                  binder;
    ReadRequestHandler      reader(service, std::move(socket), binder);

    reader.eventActivate(readFD, EV_READ);
    unlink("XX");
}
TEST_F(HTTPProtocolTest, CeckHeaders)
{
    unlink("XX");
    int         readFD = ::open("XX", O_RDWR | O_CREAT);

    std::string message = "GET /index.html HTTP/1.1\r\n"
                              "Host: ThorsAnvil.com\r\n"
                              "Content-Type: text/json\r\n"
                              "\r\n";
    ::write(readFD, message.c_str(), message.size());

    DataSocket              socket(readFD);
    Server                  service;
    Binder                  binder;
    ReadRequestHandler      reader(service, std::move(socket), binder);

    reader.eventActivate(readFD, EV_READ);
    unlink("XX");
}
TEST_F(HTTPProtocolTest, CeckHeadersMultipleValue)
{
    unlink("XX");
    int         readFD = ::open("XX", O_RDWR | O_CREAT);

    std::string message = "GET /index.html HTTP/1.1\r\n"
                              "Host: ThorsAnvil.com\r\n"
                              "Content-Type: text/json\r\n"
                              "Cookie: Cookie1\r\n"
                              "Cookie:Cookie2\r\n"
                              "Cookie: This is a long cookie with spaces\r\n"
                              "\r\n";
    ::write(readFD, message.c_str(), message.size());

    DataSocket              socket(readFD);
    Server                  service;
    Binder                  binder;
    ReadRequestHandler      reader(service, std::move(socket), binder);

    reader.eventActivate(readFD, EV_READ);
    unlink("XX");
}
TEST_F(HTTPProtocolTest, CheckBody)
{
    unlink("XX");
    int         readFD = ::open("XX", O_RDWR | O_CREAT);

    std::string message = "PUT /index.html HTTP/1.1\r\n"
                              "Content-Length: 21\r\n"
                              "\r\n"
                              "This is a body object";
    ::write(readFD, message.c_str(), message.size());

    DataSocket              socket(readFD);
    Server                  service;
    Binder                  binder;
    ReadRequestHandler      reader(service, std::move(socket), binder);

    reader.eventActivate(readFD, EV_READ);
    unlink("XX");
}

/*
TEST_F(HTTPProtocolTest, ConstructWriter)
{
    unlink("XX");
    int         readFD = ::open("XX", O_RDWR | O_CREAT);

    DataSocket              socket(readFD);
    Server                  service;
    Binder                  binder;
    std::string             uri = "thorsanvil.com/index.html";
    Headers                 headers;
    std::vector<char>       buffer;
    WriteResponseHandler  writer(service, std::move(socket), binder, Method::Get, std::move(uri), std::move(headers), std::move(buffer), nullptr, nullptr);
    unlink("XX");
}
*/

TEST_F(HTTPProtocolTest, WriterProcesses)
{
    unlink("XX");
    {
        std::ifstream input("test/data/getindex");
        std::ofstream output("XX");
        output << input.rdbuf();
    }
    int         fd = ::open("XX", O_RDWR | O_CREAT);

    bool        hitThorsAnvil = false;;
    Site        thorsAnvil;
    thorsAnvil.get("/index.html", [&hitThorsAnvil](Request&, Response&) {hitThorsAnvil = true;});

    bool        hitDefault = false;;
    Site        defaultSite;
    defaultSite.get("/index.html", [&hitDefault](Request&, Response&) {hitDefault = true;});

    Binder                  binder;
    binder.addSite("", "", std::move(defaultSite));
    binder.addSite("ThorsAnvil.com", "", std::move(thorsAnvil));

    Headers                 headers;
    headers["Host"] = "ThorsAnvil.com";

    DataSocket              socket(fd);
    Server                  service;
    std::string             uri = "/index.html";
    std::vector<char>       buffer;
    ReadRequestHandler      writer(service, std::move(socket), binder);
    writer.eventActivate(fd, EV_READ);

    ASSERT_TRUE(hitDefault);
    ASSERT_FALSE(hitThorsAnvil);
    unlink("XX");
}
