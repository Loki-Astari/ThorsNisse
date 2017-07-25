#include "NisseService.h"
#include "NisseHandler.h"
#include "ThorsNisseSocket/Socket.h"
#include "test/Action.h"
#include <gtest/gtest.h>
#include <future>
#include <iostream>

using ThorsAnvil::Nisse::NisseService;
using ThorsAnvil::Nisse::NisseHandler;
using ThorsAnvil::Socket::ConnectSocket;

TEST(NisseServiceTest, Construct)
{
    NisseService    service;
}
TEST(NisseServiceTest, MoveConstruct)
{
    NisseService    service1;
    NisseService    service2(std::move(service1));
}
TEST(NisseServiceTest, MoveAssignment)
{
    NisseService    service1;
    NisseService    service2;

    service2 = std::move(service1);
}
TEST(NisseServiceTest, StartAndShutDown)
{
    NisseService    service;
    bool            serviceFinished = false;
    auto            future = std::async(std::launch::async, [&service, &serviceFinished](){while(!serviceFinished){service.flagShutDown(); std::this_thread::yield();}});

    service.start(1);
    serviceFinished = true;
    future.wait();
}
TEST(NisseServiceTest, MoveConstructFail)
{
    NisseService    service1;
    bool            serviceStarted = false;
    auto            future = std::async(std::launch::async, [&service1, &serviceStarted](){while(!serviceStarted){std::this_thread::yield();};sleep(2);service1.flagShutDown();NisseService service2(std::move(service1));});
    serviceStarted = true;
    service1.start(1);

    ASSERT_THROW(
        future.get(),
        std::runtime_error
    );
}
TEST(NisseServiceTest, AddListener)
{
    NisseService    service;
    bool            serviceFinished = false;

    service.listenOn<Action>(9876);
    auto future = std::async(std::launch::async, [&serviceFinished](){sleep(2);while(!serviceFinished){ConnectSocket socket("127.0.0.1", 9876);}});
    service.start(1);
    serviceFinished = true;
    future.wait();
}
TEST(NisseServiceTest, AddListenerPurge)
{
    NisseService    service;
    bool            serviceFinished = false;

    service.listenOn<ActionUnReg>(9876);
    auto future = std::async(std::launch::async, [&serviceFinished](){sleep(2);while(!serviceFinished){ConnectSocket socket("127.0.0.1", 9876);}});
    service.start(1);
    serviceFinished = true;
    future.wait();
}
