#include "NisseService.h"
#include "NisseHandler.h"
#include "ThorsNisseCoreSocket/Socket.h"
#include "test/Action.h"
#include <gtest/gtest.h>
#include <future>
#include <iostream>

using ThorsAnvil::Nisse::Core::Service::NisseService;
using ThorsAnvil::Nisse::Core::Service::NisseHandler;
using ThorsAnvil::Nisse::Core::Socket::ConnectSocket;

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
    bool            finished = false;
    service.addTimer(1, [&service, &finished]()
    {
        service.flagShutDown();
        finished = true;
    });

    service.start(1);
    ASSERT_TRUE(finished);
}
TEST(NisseServiceTest, MoveConstructFail)
{
    NisseService    service1;
    bool            finished = false;
    service1.addTimer(1, [&service1, &finished]()
    {
        ASSERT_THROW(
            NisseService service2(std::move(service1)),
            std::runtime_error
        );
        service1.flagShutDown();
        finished = true;
    });
    service1.start(1);
    ASSERT_TRUE(finished);
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
