#include "Service.h"
#include "ThorsNisseCoreSocket/Socket.h"
#include "EventUtil.h"
#include "test/Action.h"
#include <gtest/gtest.h>
#include "coverage/ThorMock.h"
#include <future>
#include <iostream>

using ThorsAnvil::Nisse::Core::Service::Server;
using ThorsAnvil::Nisse::Core::Service::LibEventConfig;
using ThorsAnvil::Nisse::Core::Service::LibEventBase;
using ThorsAnvil::Nisse::Core::Service::TimeVal;
using ThorsAnvil::Nisse::Core::Socket::ConnectSocket;

TEST(ServerTest, Construct)
{
    Server    service;
}
TEST(ServerTest, MoveConstruct)
{
    Server    service1;
    Server    service2(std::move(service1));
}
TEST(ServerTest, MoveAssignment)
{
    Server    service1;
    Server    service2;

    service2 = std::move(service1);
}
TEST(ServerTest, StartAndShutDown)
{
    Server    service;
    bool      finished = false;
    service.addTimer(0.1, [&service, &finished]()
    {
        service.flagShutDown();
        finished = true;
    });

    service.start(0.1);
    ASSERT_TRUE(finished);
}
TEST(ServerTest, MoveConstructFail)
{
    Server    service1;
    bool      finished = false;
    service1.addTimer(0.1, [&service1, &finished]()
    {
        ASSERT_THROW(
            Server service2(std::move(service1)),
            std::runtime_error
        );
        service1.flagShutDown();
        finished = true;
    });
    service1.start(0.1);
    ASSERT_TRUE(finished);
}
TEST(ServerTest, AddListener)
{
    Server    service;
    bool      serviceFinished = false;

    service.listenOn<Action>(9876);
    auto future = std::async(std::launch::async, [&serviceFinished](){usleep(200000);while(!serviceFinished){ConnectSocket socket("127.0.0.1", 9876);}});
    service.start(0.1);
    serviceFinished = true;
    future.wait();
}
TEST(ServerTest, AddListenerPurge)
{
    Server    service;
    bool      serviceFinished = false;

    service.listenOn<ActionUnReg>(9876);
    auto future = std::async(std::launch::async, [&serviceFinished](){usleep(200000);while(!serviceFinished){ConnectSocket socket("127.0.0.1", 9876);}});
    service.start(0.1);
    serviceFinished = true;
    future.wait();
}

TEST(ServerTestExceptions, EventBaseFail)
{
    MOCK_SYS(event_base_new_with_config, [](LibEventConfig const*){return nullptr;});

    ASSERT_THROW(
        Server    service,
        std::runtime_error
    );
}
TEST(ServerTestExceptions, ShutDownFail)
{
    MOCK_SYS(event_base_loopbreak, [](LibEventBase*){return -1;});

    ASSERT_THROW(
        Server    server;
        server.flagShutDown(),
        std::runtime_error
    );
}
TEST(ServerTestExceptions, RunLoopFail)
{
    MOCK_SYS(event_base_loopexit, [](LibEventBase*, TimeVal const*){return -1;});

    ASSERT_THROW(
        Server    server;
        server.start(0.2),
        std::runtime_error
    );
}
TEST(ServerTestExceptions, RunLoopGetEventFail)
{
    MOCK_SYS(event_base_dispatch, [](LibEventBase*){return -1;});

    ASSERT_THROW(
        Server    server;
        server.start(0.2),
        std::runtime_error
    );
}
TEST(ServerTestExceptions, RunLoopGetEventNoEvents)
{
    MOCK_SYS(event_base_dispatch, [](LibEventBase*){return 1;});

    ASSERT_THROW(
        Server    server;
        server.start(0.2),
        std::runtime_error
    );
}
TEST(ServerTestExceptions, RunLoopGetEventUnknown)
{
    MOCK_SYS(event_base_dispatch, [](LibEventBase*){return 9998;});

    ASSERT_THROW(
        Server    server;
        server.start(0.2),
        std::runtime_error
    );
}
