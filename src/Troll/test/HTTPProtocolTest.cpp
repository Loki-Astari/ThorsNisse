#include "HTTPProtocol.h"
#include <gtest/gtest.h>
#include "Binder.h"
#include "ThorsNisse/NisseService.h"
#include "ThorsNisseSocket/Socket.h"
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


using ThorsAnvil::Nisse::ProtocolHTTP::ReadRequestHandler;
using ThorsAnvil::Nisse::ProtocolHTTP::Request;
using ThorsAnvil::Nisse::ProtocolHTTP::Response;
using ThorsAnvil::Nisse::ProtocolHTTP::Method;
using ThorsAnvil::Nisse::ProtocolHTTP::Headers;
using ThorsAnvil::Nisse::ProtocolHTTP::Binder;
using ThorsAnvil::Nisse::ProtocolHTTP::Site;
using ThorsAnvil::Nisse::NisseService;
using ThorsAnvil::Socket::DataSocket;

using namespace std;
 
// A new one of these is created for each test
class HTTPProtocolTest : public testing::Test
{
    public:
        virtual void SetUp()
        {
            NisseService::ignore("epoll");
        }

        virtual void TearDown()
        {
            NisseService::ignore();
        }
};
 
TEST_F(HTTPProtocolTest, Construct)
{
    unlink("XX");
    int         readFD = ::open("XX", O_RDWR | O_CREAT);

    DataSocket              socket(readFD);
    NisseService            service;
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
    NisseService            service;
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
    NisseService            service;
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
    NisseService            service;
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
    NisseService            service;
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
    NisseService            service;
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
    NisseService            service;
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
    NisseService            service;
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
    NisseService            service;
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
    NisseService            service;
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
    std::cerr << "FD -> " << fd << "\n\n";

    bool        hitThorsAnvil = false;;
    Site        thorsAnvil;
    thorsAnvil.get("/index.html", [&hitThorsAnvil](Request&, Response&) {hitThorsAnvil = true;});

    bool        hitDefault = false;;
    Site        defaultSite;
    defaultSite.get("/index.html", [&hitDefault](Request&, Response&) {hitDefault = true;});

    Binder                  binder;
    binder.addSite("", std::move(defaultSite));
    binder.addSite("ThorsAnvil.com", std::move(thorsAnvil));

    Headers                 headers;
    headers["Host"] = "ThorsAnvil.com";

    DataSocket              socket(fd);
    NisseService            service;
    std::string             uri = "/index.html";
    std::vector<char>       buffer;
    ReadRequestHandler      writer(service, std::move(socket), binder);
    writer.eventActivate(fd, EV_READ);

    ASSERT_TRUE(hitDefault);
    ASSERT_FALSE(hitThorsAnvil);
    unlink("XX");
}
