#include "Server.h"
#include "Handler.h"
#include "EventUtil.h"
#include "ThorsNisseCoreSocket/Socket.h"
#include "test/Action.h"
#include <gtest/gtest.h>
#include "coverage/ThorMock.h"
#include <future>
#include <iostream>

using ThorsAnvil::Nisse::Core::Service::Server;
using ThorsAnvil::Nisse::Core::Service::LibSocketId;
using ThorsAnvil::Nisse::Core::Service::HandlerBase;
using ThorsAnvil::Nisse::Core::Service::LibEvent;
using ThorsAnvil::Nisse::Core::Service::LibEventBase;
using ThorsAnvil::Nisse::Core::Service::LibSocketId;
using ThorsAnvil::Nisse::Core::Service::TimeVal;
using ThorsAnvil::Nisse::Core::Socket::ConnectSocket;


TEST(HandlerTest, SuspendResume)
{
    Server    server;
    std::tuple<bool, bool, bool>   hits {false, false, false};

    server.listenOn<TestHandler>(9876, hits);
    auto future = std::async(std::launch::async, [&hits](){usleep(200000);while(!std::get<0>(hits)){ConnectSocket socket("127.0.0.1", 9876);}});

    server.start();
    future.wait();

    ASSERT_TRUE(std::get<0>(hits));
    ASSERT_TRUE(std::get<1>(hits));
    ASSERT_TRUE(std::get<2>(hits));
}

TEST(HandlerTest, InHandlerFalse)
{
    ASSERT_FALSE(Server::inHandler());
}

TEST(HandlerTest, InHandlerTrue)
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

TEST(HandlerTestException, HandlerCreationFailsFirstNew)
{
    MOCK_SYS(event_new, [](LibEventBase*, LibSocketId, short, event_callback_fn, void*){return nullptr;});

    Server    server;
    std::tuple<bool, bool, bool>   hits {false, false, false};

    ASSERT_THROW(
        server.listenOn<TestHandler>(9876, hits),
        std::runtime_error
    );
}
TEST(HandlerTestException, HandlerCreationFailsSecondNew)
{
    int count = 0;
    MOCK_SYS(event_new, [&count](LibEventBase* eb, LibSocketId sid, short f, event_callback_fn cb, void* data) -> LibEvent* {++count;if (count == 1){return ::event_new(eb, sid, f, cb, data);}return nullptr;});

    Server    server;
    std::tuple<bool, bool, bool>   hits {false, false, false};

    ASSERT_THROW(
        server.listenOn<TestHandler>(9876, hits),
        std::runtime_error
    );
}

TEST(HandlerTestException, HandlerDeletionFailsFirstDel)
{
    int count = 0;
    MOCK_SYS(event_del, [&count](LibEvent* e){++count;if (count == 1){return -1;}return ::event_del(e);});

    auto doTest = [](){
        Server      server;
        Test2Handler handler(server, 67, EV_READ | EV_WRITE);
        handler.dropEvent();
    };
    ASSERT_THROW(
        doTest(),
        std::runtime_error
    );
}
TEST(HandlerTestException, HandlerDeletionFailsSecondDel)
{
    int count = 0;
    MOCK_SYS(event_del, [&count](LibEvent* e){++count;if (count == 2){return -1;}return ::event_del(e);});

    auto doTest = [](){
        Server      server;
        Test2Handler handler(server, 67, EV_READ | EV_WRITE);
        handler.dropEvent();
    };
    ASSERT_THROW(
        doTest(),
        std::runtime_error
    );
}

TEST(HandlerTestException, HandlerAddFailsFirstDel)
{
    int count = 0;
    MOCK_SYS(event_add, [&count](LibEvent* e, TimeVal const* t){++count;if (count == 1){return -1;}return ::event_add(e, t);});

    auto doTest = [](){
        Server      server;
        Test2Handler handler(server, 67, EV_READ | EV_WRITE);
    };
    ASSERT_THROW(
        doTest(),
        std::runtime_error
    );
}
TEST(HandlerTestException, HandlerAddFailsSecondDel)
{
    int count = 0;
    MOCK_SYS(event_add, [&count](LibEvent* e, TimeVal const* t){++count;if (count == 2){return -1;}return ::event_add(e, t);});

    auto doTest = [](){
        Server      server;
        Test2Handler handler(server, 67, EV_READ | EV_WRITE);
    };
    ASSERT_THROW(
        doTest(),
        std::runtime_error
    );
}


