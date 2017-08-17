#include "Server.h"
#include "Handler.h"
#include "ThorsNisseCoreSocket/Socket.h"
#include "test/Action.h"
#include <gtest/gtest.h>
#include <future>
#include <iostream>

using ThorsAnvil::Nisse::Core::Service::Server;
using ThorsAnvil::Nisse::Core::Service::Handler;
using ThorsAnvil::Nisse::Core::Socket::ConnectSocket;

TEST(CoreServiceServerTest, Construct)
{
    Server    service;
}
TEST(CoreServiceServerTest, MoveConstruct)
{
    Server    service1;
    Server    service2(std::move(service1));
}
TEST(CoreServiceServerTest, MoveAssignment)
{
    Server    service1;
    Server    service2;

    service2 = std::move(service1);
}
TEST(CoreServiceServerTest, StartAndShutDown)
{
    Server    service;
    bool      finished = false;
    service.addTimer(1, [&service, &finished]()
    {
        service.flagShutDown();
        finished = true;
    });

    service.start(1);
    ASSERT_TRUE(finished);
}
TEST(CoreServiceServerTest, MoveConstructFail)
{
    Server    service1;
    bool      finished = false;
    service1.addTimer(1, [&service1, &finished]()
    {
        ASSERT_THROW(
            Server service2(std::move(service1)),
            std::runtime_error
        );
        service1.flagShutDown();
        finished = true;
    });
    service1.start(1);
    ASSERT_TRUE(finished);
}
TEST(CoreServiceServerTest, AddListener)
{
    Server    service;
    bool      serviceFinished = false;

    service.listenOn<Action>(9876);
    auto future = std::async(std::launch::async, [&serviceFinished](){sleep(2);while(!serviceFinished){ConnectSocket socket("127.0.0.1", 9876);}});
    service.start(1);
    serviceFinished = true;
    future.wait();
}
TEST(CoreServiceServerTest, AddListenerPurge)
{
    Server    service;
    bool      serviceFinished = false;

    service.listenOn<ActionUnReg>(9876);
    auto future = std::async(std::launch::async, [&serviceFinished](){sleep(2);while(!serviceFinished){ConnectSocket socket("127.0.0.1", 9876);}});
    service.start(1);
    serviceFinished = true;
    future.wait();
}
