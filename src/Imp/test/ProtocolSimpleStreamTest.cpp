#include "ProtocolSimpleStream.h"
#include "ProtocolSimple.h"
#include "ThorsNisse/NisseService.h"
#include "ThorsNisse/NisseTimer.h"
#include "ThorsNisseSocket/Socket.h"
#include "ThorsNisseSocket/Utility.h"
#include <gtest/gtest.h>
#include <future>
#include <unistd.h>
#include <event2/util.h>
#include <boost/coroutine/asymmetric_coroutine.hpp>

using ThorsAnvil::Nisse::NisseService;
using ThorsAnvil::Nisse::NisseTimer;
using ThorsAnvil::Nisse::ProtocolSimple::Message;
using ThorsAnvil::Nisse::ProtocolSimple::ReadMessageHandler;
using ThorsAnvil::Nisse::ProtocolSimple::ReadMessageStreamHandler;
using ThorsAnvil::Nisse::ProtocolSimple::WriteMessageStreamHandler;
using ThorsAnvil::Socket::ConnectSocket;

using CoRoutine = boost::coroutines::asymmetric_coroutine<void>::pull_type;
using Yield     = boost::coroutines::asymmetric_coroutine<void>::push_type;

std::size_t x1 = 0x0123456789ABCDEF;
std::size_t size;
std::size_t x2 = 0xFEDCBA9876543210;
int testPort = 9870;
TEST(ProtocolSimpleStreamTest, ReadMessageHandler)
{
	NisseService	service;
    bool            finished = false;

	service.listenOn<ReadMessageStreamHandler>(testPort);
    service.addTimer(1, [&service, &finished]()
    {
        if (finished)
        {
            service.flagShutDown();
        }
    });
        
    auto future = std::async([&finished]()
    {
        std::string message = "This is a test Message";
        size    = message.size();

        ConnectSocket connect("127.0.0.1", testPort);

        connect.putMessageData(reinterpret_cast<char*>(&size), sizeof(size));
        connect.putMessageData(message.c_str(), size);

        connect.getMessageData(reinterpret_cast<char*>(&size), sizeof(size));
        message.resize(size);
        connect.getMessageData(&message[0], size);

        finished = true;
        return message;
    });

	service.start(1);
    std::string result = future.get();

    ASSERT_EQ("This is a test Message" + WriteMessageStreamHandler::messageSuffix, result);
}

TEST(ProtocolSimpleStreamTest, ReadMessageHandlerPartialSize)
{
	NisseService	service;
    bool            finished = false;

	service.listenOn<ReadMessageStreamHandler>(testPort);
    service.addTimer(1, [&service, &finished]()
    {
        if (finished)
        {
            service.flagShutDown();
        }
    });
        
	auto future = std::async([&finished](){
        sleep(1);
		ConnectSocket connect("127.0.0.1", testPort);
		std::string message = "This is a test Message";
		std::size_t size    = message.size();
		connect.putMessageData(reinterpret_cast<char*>(&size), 2);
        connect.putMessageClose();

        connect.getMessageData(reinterpret_cast<char*>(&size), sizeof(size));
        message.resize(size);
        connect.getMessageData(&message[0], size);

        finished = true;
        return message;
	});

	service.start(1);
    std::string result = future.get();

    ASSERT_EQ(ReadMessageStreamHandler::failToReadMessage + WriteMessageStreamHandler::messageSuffix, result);
}

TEST(ProtocolSimpleStreamTest, ReadMessageHandlerPartialSizeInTwoChunks)
{
	NisseService	service;
    bool            finished = false;

	service.listenOn<ReadMessageStreamHandler>(testPort);
    service.addTimer(1, [&service, &finished]()
    {
        if (finished)
        {
            service.flagShutDown();
        }
    });
        
	auto future = std::async([&finished](){
		ConnectSocket connect("127.0.0.1", testPort);
		std::string message = "This is a test Message";
		std::size_t size    = message.size();
		connect.putMessageData(reinterpret_cast<char*>(&size), 2);
        sleep(1);
		connect.putMessageData(reinterpret_cast<char*>(&size), 1, 2);
        connect.putMessageClose();

        connect.getMessageData(reinterpret_cast<char*>(&size), sizeof(size));
        message.resize(size);
        connect.getMessageData(&message[0], size);

        finished = true;
        return message;
	});

	service.start(1);
    std::string result = future.get();

    ASSERT_EQ(ReadMessageStreamHandler::failToReadMessage + WriteMessageStreamHandler::messageSuffix, result);
}
TEST(ProtocolSimpleStreamTest, ReadMessageHandlerSizeInTwoChunks)
{
	NisseService	service;
    bool            finished = false;

	service.listenOn<ReadMessageStreamHandler>(testPort);
    service.addTimer(1, [&service, &finished]()
    {
        if (finished)
        {
            service.flagShutDown();
        }
    });
        
	auto future = std::async([&finished](){
		ConnectSocket connect("127.0.0.1", testPort);
		std::string message = "This is a test Message";
		std::size_t size    = message.size();
		connect.putMessageData(reinterpret_cast<char*>(&size), 2);
        sleep(1);
		connect.putMessageData(reinterpret_cast<char*>(&size), sizeof(size), 2);
		connect.putMessageData(message.c_str(), size);

        connect.getMessageData(reinterpret_cast<char*>(&size), sizeof(size));
        message.resize(size);
        connect.getMessageData(&message[0], size);

        finished = true;
        return message;
	});

	service.start(1);
    std::string result = future.get();

    ASSERT_EQ("This is a test Message" + WriteMessageStreamHandler::messageSuffix, result);
}
TEST(ProtocolSimpleStreamTest, ReadMessageHandlerPartialMessage)
{
	NisseService	service;
    bool            finished = false;

	service.listenOn<ReadMessageStreamHandler>(testPort);
    service.addTimer(1, [&service, &finished]()
    {
        if (finished)
        {
            service.flagShutDown();
        }
    });
        
	auto future = std::async([&finished](){
		ConnectSocket connect("127.0.0.1", testPort);
		std::string message = "This is a test Message";
		std::size_t size    = message.size();
		connect.putMessageData(reinterpret_cast<char*>(&size), sizeof(size));
		connect.putMessageData(message.c_str(), size/2);
        connect.putMessageClose();

        connect.getMessageData(reinterpret_cast<char*>(&size), sizeof(size));
        message.resize(size);
        connect.getMessageData(&message[0], size);

        finished = true;
        return message;
	});

	service.start(1);
    std::string result = future.get();

    ASSERT_EQ(ReadMessageStreamHandler::failToReadMessage + WriteMessageStreamHandler::messageSuffix, result);
}
TEST(ProtocolSimpleStreamTest, ReadMessageHandlerPartialMessageInTwoChunks)
{
	NisseService	service;
    bool            finished = false;

	service.listenOn<ReadMessageStreamHandler>(testPort);
    service.addTimer(1, [&service, &finished]()
    {
        if (finished)
        {
            service.flagShutDown();
        }
    });
        
	auto future = std::async([&finished](){
		ConnectSocket connect("127.0.0.1", testPort);
		std::string message = "This is a test Message";
		std::size_t size    = message.size();
		connect.putMessageData(reinterpret_cast<char*>(&size), sizeof(size));
		connect.putMessageData(message.c_str(), size/2);
        sleep(1);
		connect.putMessageData(message.c_str(), size, size/4*3);
        connect.putMessageClose();

        connect.getMessageData(reinterpret_cast<char*>(&size), sizeof(size));
        message.resize(size);
        connect.getMessageData(&message[0], size);

        finished = true;
        return message;
	});

	service.start(1);
    std::string result = future.get();

    ASSERT_EQ(ReadMessageStreamHandler::failToReadMessage + WriteMessageStreamHandler::messageSuffix, result);
}
TEST(ProtocolSimpleStreamTest, ReadMessageHandlerMessageInTwoChunks)
{
	NisseService	service;
    bool            finished = false;

	service.listenOn<ReadMessageStreamHandler>(testPort);
    service.addTimer(1, [&service, &finished]()
    {
        if (finished)
        {
            service.flagShutDown();
        }
    });

	auto future = std::async([&finished](){
		ConnectSocket connect("127.0.0.1", testPort);
		std::string message = "This is a test Message";
		std::size_t size    = message.size();
		connect.putMessageData(reinterpret_cast<char*>(&size), sizeof(size));
		connect.putMessageData(message.c_str(), size/2);
        sleep(1);
		connect.putMessageData(message.c_str(), size, size/2);
        connect.putMessageClose();

        connect.getMessageData(reinterpret_cast<char*>(&size), sizeof(size));
        message.resize(size);
        connect.getMessageData(&message[0], size);

        finished = true;
        return message;
	});

	service.start(1);

    std::string result = future.get();
    ASSERT_EQ("This is a test Message" + WriteMessageStreamHandler::messageSuffix, result);
}
TEST(ProtocolSimpleStreamTest, WriteMessageHandler)
{
	NisseService	service;
    bool            finished = false;
    Message         message;

    message.message = "A Write Test";

	service.listenOn<WriteMessageStreamHandler>(testPort, message);
    service.addTimer(1, [&service, &finished]()
    {
        if (finished)
        {
            service.flagShutDown();
        }
    });
        
	auto future = std::async([&finished](){
		ConnectSocket connect("127.0.0.1", testPort);
		std::string message;
		std::size_t size    = message.size();
        connect.getMessageData(reinterpret_cast<char*>(&size), sizeof(size));
        message.resize(size);
        connect.getMessageData(&message[0], size);

        finished = true;
        return message;
	});

	service.start(1);
    std::string result = future.get();

    ASSERT_EQ("A Write Test" + WriteMessageStreamHandler::messageSuffix, result);
}
