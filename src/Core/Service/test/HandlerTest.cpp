#include "Server.h"
#include "Handler.h"
#include "ThorsNisseCoreSocket/Socket.h"
#include "test/Action.h"
#include <gtest/gtest.h>
#include <future>
#include <iostream>

using ThorsAnvil::Nisse::Core::Service::Server;
using ThorsAnvil::Nisse::Core::Service::LibSocketId;
using ThorsAnvil::Nisse::Core::Service::Handler;
using ThorsAnvil::Nisse::Core::Socket::ConnectSocket;

//        void activateEventHandlers(LibSocketId sockId, short eventType);        // The C-Callback point.


TEST(CoreServiceHandlerTest, SuspendResume)
{
    Server    server;
    std::tuple<bool, bool, bool>   hits = {false, false, false};

    server.listenOn<TestHandler>(9876, hits);
    auto future = std::async(std::launch::async, [&hits](){usleep(200000);while(!std::get<0>(hits)){ConnectSocket socket("127.0.0.1", 9876);}});

    server.start();
    future.wait();

    ASSERT_TRUE(std::get<0>(hits));
    ASSERT_TRUE(std::get<1>(hits));
    ASSERT_TRUE(std::get<2>(hits));
}

TEST(CoreServiceHandlerTest, InHandlerFalse)
{
    ASSERT_FALSE(Server::inHandler());
}

TEST(CoreServiceHandlerTest, InHandlerTrue)
{
    Server                                      server;
    std::tuple<bool, std::function<void(Server&)>>     active
    {
        false,
        [](Server& server)
        {
            ASSERT_TRUE(Server::inHandler());
            ASSERT_EQ(&server, &Server::getCurrentHandler());
        }
    };

    server.listenOn<InHandlerTest>(9876, active);
    auto future = std::async(std::launch::async, [&active](){usleep(200000);while(!std::get<0>(active)){ConnectSocket socket("127.0.0.1", 9876);}});

    server.start();
    future.wait();

}


