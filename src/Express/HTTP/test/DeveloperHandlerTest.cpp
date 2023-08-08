#if HAVE_DEVLOADER
#include "DeveloperHandler.h"
#include <gtest/gtest.h>
#include "coverage/ThorMock.h"
#include "ThorsNisseCoreService/Server.h"
#include "ThorsNisseCoreService/Handler.h"
#include "ThorsSocket/Socket.h"
#include <future>

using ThorsAnvil::Nisse::Core::Service::Server;
using ThorsAnvil::ThorsSocket::ConnectSocketNormal;
using ThorsAnvil::Nisse::Protocol::HTTP::DynamicSiteLoader;
using ThorsAnvil::Nisse::Protocol::HTTP::DeveloperHandler;

TEST(DeveloperHandlerTest, Construct)
{
    Server              server;
    DynamicSiteLoader   siteLoader(server);
    server.addTimer(0.5, [&server]()
    {
        server.flagShutDown();
    });

    auto future = std::async(std::launch::async, [](){
        usleep(200000);
        ConnectSocketNormal socket("127.0.0.1", 8078);
        std::string message = "PUT / HTTP/1.1\r\nContent-Length: 67\r\n\r\n" R"({"action": "a", "lib": "l", "host": "h", "base": "b", "port": 8000})";
        socket.putMessageData(message.c_str(), message.size());
        socket.putMessageClose();
    });

    server.listenOn<DeveloperHandler>(8078, siteLoader);
    server.start(0.1);
    future.wait();
}
TEST(DeveloperHandlerTest, LoadLoadable)
{
    Server              server;
    DynamicSiteLoader   siteLoader(server);
    server.addTimer(0.5, [&server]()
    {
        server.flagShutDown();
    });

    auto future = std::async(std::launch::async, [](){
        usleep(200000);
        ConnectSocketNormal socket("127.0.0.1", 8078);
        std::string message = "PUT / HTTP/1.1\r\nContent-Length: 67\r\n\r\n" R"({"action": "Load", "lib": "TestLib/Loadable/Loadable.dylib", "host": "h", "base": "b", "port": 8000})";
        socket.putMessageData(message.c_str(), message.size());
        socket.putMessageClose();
    });

    server.listenOn<DeveloperHandler>(8078, siteLoader);
    server.start(0.1);
    future.wait();
}
TEST(DeveloperHandlerTest, UnLoadLoadable)
{
    Server              server;
    DynamicSiteLoader   siteLoader(server);
    server.addTimer(0.5, [&server]()
    {
        server.flagShutDown();
    });

    auto future = std::async(std::launch::async, [](){
        usleep(200000);
        {
            ConnectSocketNormal socket("127.0.0.1", 8078);
            std::string message = "PUT / HTTP/1.1\r\nContent-Length: 100\r\n\r\n" R"({"action": "Load", "lib": "TestLib/Loadable/Loadable.dylib", "host": "h", "base": "b", "port": 8000})";
            socket.putMessageData(message.c_str(), message.size());
            socket.putMessageClose();
        }
        usleep(200000);
        {
            ConnectSocketNormal socket("127.0.0.1", 8078);
            std::string message = "PUT / HTTP/1.1\r\nContent-Length: 102\r\n\r\n" R"({"action": "Unload", "lib": "TestLib/Loadable/Loadable.dylib", "host": "h", "base": "b", "port": 8000})";
            socket.putMessageData(message.c_str(), message.size());
            socket.putMessageClose();
        }
    });

    server.listenOn<DeveloperHandler>(8078, siteLoader);
    server.start(0.1);
    future.wait();
}
TEST(DeveloperHandlerExceptionTest, UnLoadFail)
{
    Server              server;
    DynamicSiteLoader   siteLoader(server);
    server.addTimer(0.5, [&server]()
    {
        server.flagShutDown();
    });

    auto future = std::async(std::launch::async, [](){
        usleep(200000);
        {
            ConnectSocketNormal socket("127.0.0.1", 8078);
            std::string message = "PUT / HTTP/1.1\r\nContent-Length: 102\r\n\r\n" R"({"action": "Unload", "lib": "TestLib/Loadable/Loadable.dylib", "host": "h", "base": "b", "port": 8000})";
            socket.putMessageData(message.c_str(), message.size());
            socket.putMessageClose();
        }
    });

    server.listenOn<DeveloperHandler>(8078, siteLoader);
    server.start(0.1);
    future.wait();
}
#endif
