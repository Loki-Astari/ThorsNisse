#include "ProtocolSimpleStream.h"
#include "ProtocolSimple.h"
#include "ThorsNisseCoreService/Server.tpp"
#include "ThorsNisseCoreService/Handler.tpp"
#include "ThorsNisseCoreService/ServerHandler.tpp"
#include "ThorsSocket/Socket.h"
#include <gtest/gtest.h>
#include <future>
#include <unistd.h>
#include <event2/util.h>

using ThorsAnvil::Nisse::Core::Service::Server;
using ThorsAnvil::ThorsSocket::ConnectSocketNormal;
using ThorsAnvil::Nisse::Protocol::Simple::Message;
using ThorsAnvil::Nisse::Protocol::Simple::ReadMessageHandler;
using ThorsAnvil::Nisse::Protocol::Simple::ReadMessageStreamHandler;
using ThorsAnvil::Nisse::Protocol::Simple::WriteMessageStreamHandler;

TEST(ProtocolSimpleStreamTest, ReadMessageHandler)
{
    Server          service;
    bool            finished = false;

    service.listenOn<ReadMessageStreamHandler>(9870);
    service.addTimer(0.1, [&service, &finished]()
    {
        if (finished)
        {
            service.flagShutDown();
        }
    });

    auto future = std::async(std::launch::async, [&finished]()
    {
        std::string message = "Test function ReadMessageHandler";
        std::size_t size    = message.size();

        ConnectSocketNormal connect("127.0.0.1", 9870);

        connect.putMessageData(reinterpret_cast<char*>(&size), sizeof(size));
        connect.putMessageData(message.c_str(), size);

        connect.getMessageData(reinterpret_cast<char*>(&size), sizeof(size));
        message.resize(size);
        connect.getMessageData(&message[0], size);

        finished = true;
        return message;
    });

    service.start(0.1);
    std::string result = future.get();

    ASSERT_EQ("Test function ReadMessageHandler" + WriteMessageStreamHandler::messageSuffix, result);
}

TEST(ProtocolSimpleStreamTest, ReadMessageHandlerPartialSize)
{
    Server          service;
    bool            finished = false;

    service.listenOn<ReadMessageStreamHandler>(9871);
    service.addTimer(0.1, [&service, &finished]()
    {
        if (finished)
        {
            service.flagShutDown();
        }
    });

    auto future = std::async(std::launch::async, [&finished](){
        usleep(100000);
        ConnectSocketNormal connect("127.0.0.1", 9871);
        std::string message = "Test function ReadMessageHandlerPartialSize";
        std::size_t size    = message.size();
        connect.putMessageData(reinterpret_cast<char*>(&size), 2);
        connect.putMessageClose();

        connect.getMessageData(reinterpret_cast<char*>(&size), sizeof(size));
        message.resize(size);
        connect.getMessageData(&message[0], size);

        finished = true;
        return message;
    });

    service.start(0.1);
    std::string result = future.get();

    ASSERT_EQ(ReadMessageStreamHandler::failToReadMessage + WriteMessageStreamHandler::messageSuffix, result);
}

TEST(ProtocolSimpleStreamTest, ReadMessageHandlerPartialSizeInTwoChunks)
{
    Server          service;
    bool            finished = false;

    service.listenOn<ReadMessageStreamHandler>(9872);
    service.addTimer(0.1, [&service, &finished]()
    {
        if (finished)
        {
            service.flagShutDown();
        }
    });

    auto future = std::async(std::launch::async, [&finished](){
        ConnectSocketNormal connect("127.0.0.1", 9872);
        std::string message = "Test fucntion ReadMessageHandlerPartialSizeInTwoChunks";
        std::size_t size    = message.size();
        connect.putMessageData(reinterpret_cast<char*>(&size), 2);
        usleep(100000);
        connect.putMessageData(reinterpret_cast<char*>(&size), 1, 2);
        connect.putMessageClose();

        connect.getMessageData(reinterpret_cast<char*>(&size), sizeof(size));
        message.resize(size);
        connect.getMessageData(&message[0], size);

        finished = true;
        return message;
    });

    service.start(0.1);
    std::string result = future.get();

    ASSERT_EQ(ReadMessageStreamHandler::failToReadMessage + WriteMessageStreamHandler::messageSuffix, result);
}
TEST(ProtocolSimpleStreamTest, ReadMessageHandlerSizeInTwoChunks)
{
    Server          service;
    bool            finished = false;

    service.listenOn<ReadMessageStreamHandler>(9873);
    service.addTimer(0.1, [&service, &finished]()
    {
        if (finished)
        {
            service.flagShutDown();
        }
    });

    auto future = std::async(std::launch::async, [&finished](){
        ConnectSocketNormal connect("127.0.0.1", 9873);
        std::string message = "Test function ReadMessageHandlerSizeInTwoChunks";
        std::size_t size    = message.size();
        connect.putMessageData(reinterpret_cast<char*>(&size), 2);
        usleep(100000);
        connect.putMessageData(reinterpret_cast<char*>(&size), sizeof(size), 2);
        connect.putMessageData(message.c_str(), size);

        connect.getMessageData(reinterpret_cast<char*>(&size), sizeof(size));
        message.resize(size);
        connect.getMessageData(&message[0], size);

        finished = true;
        return message;
    });

    service.start(0.1);
    std::string result = future.get();

    ASSERT_EQ("Test function ReadMessageHandlerSizeInTwoChunks" + WriteMessageStreamHandler::messageSuffix, result);
}
TEST(ProtocolSimpleStreamTest, ReadMessageHandlerPartialMessage)
{
    Server          service;
    bool            finished = false;

    service.listenOn<ReadMessageStreamHandler>(9874);
    service.addTimer(0.1, [&service, &finished]()
    {
        if (finished)
        {
            service.flagShutDown();
        }
    });

    auto future = std::async(std::launch::async, [&finished](){
        ConnectSocketNormal connect("127.0.0.1", 9874);
        std::string message = "Test function ReadMessageHandlerPartialMessage";
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

    service.start(0.1);
    std::string result = future.get();

    ASSERT_EQ(ReadMessageStreamHandler::failToReadMessage + WriteMessageStreamHandler::messageSuffix, result);
}
TEST(ProtocolSimpleStreamTest, ReadMessageHandlerPartialMessageInTwoChunks)
{
    Server          service;
    bool            finished = false;

    service.listenOn<ReadMessageStreamHandler>(9875);
    service.addTimer(0.1, [&service, &finished]()
    {
        if (finished)
        {
            service.flagShutDown();
        }
    });

    auto future = std::async(std::launch::async, [&finished](){
        ConnectSocketNormal connect("127.0.0.1", 9875);
        std::string message = "Test function ReadMessageHandlerPartialMessageInTwoChunks";
        std::size_t size    = message.size();
        connect.putMessageData(reinterpret_cast<char*>(&size), sizeof(size));
        connect.putMessageData(message.c_str(), size/2);
        usleep(100000);
        connect.putMessageData(message.c_str(), size, size/4*3);
        connect.putMessageClose();

        connect.getMessageData(reinterpret_cast<char*>(&size), sizeof(size));
        message.resize(size);
        connect.getMessageData(&message[0], size);

        finished = true;
        return message;
    });

    service.start(0.1);
    std::string result = future.get();

    ASSERT_EQ(ReadMessageStreamHandler::failToReadMessage + WriteMessageStreamHandler::messageSuffix, result);
}
TEST(ProtocolSimpleStreamTest, ReadMessageHandlerMessageInTwoChunks)
{
    Server          service;
    bool            finished = false;

    service.listenOn<ReadMessageStreamHandler>(9876);
    service.addTimer(0.1, [&service, &finished]()
    {
        if (finished)
        {
            service.flagShutDown();
        }
    });

    auto future = std::async(std::launch::async, [&finished](){
        ConnectSocketNormal connect("127.0.0.1", 9876);
        std::string message = "Test function ReadMessageHandlerMessageInTwoChunks";
        std::size_t size    = message.size();
        connect.putMessageData(reinterpret_cast<char*>(&size), sizeof(size));
        connect.putMessageData(message.c_str(), size/2);
        usleep(100000);
        connect.putMessageData(message.c_str(), size, size/2);
        connect.putMessageClose();

        connect.getMessageData(reinterpret_cast<char*>(&size), sizeof(size));
        message.resize(size);
        connect.getMessageData(&message[0], size);

        finished = true;
        return message;
    });

    service.start(0.1);

    std::string result = future.get();
    ASSERT_EQ("Test function ReadMessageHandlerMessageInTwoChunks" + WriteMessageStreamHandler::messageSuffix, result);
}
TEST(ProtocolSimpleStreamTest, WriteMessageHandler)
{
    Server          service;
    bool            finished = false;
    Message         message;

    message.message = "A Write Test";

    service.listenOn<WriteMessageStreamHandler>(9877, message);
    service.addTimer(0.1, [&service, &finished]()
    {
        if (finished)
        {
            service.flagShutDown();
        }
    });

    auto future = std::async(std::launch::async, [&finished](){
        ConnectSocketNormal connect("127.0.0.1", 9877);
        std::string message;
        std::size_t size    = message.size();
        connect.getMessageData(reinterpret_cast<char*>(&size), sizeof(size));
        message.resize(size);
        connect.getMessageData(&message[0], size);

        finished = true;
        return message;
    });

    service.start(0.1);
    std::string result = future.get();

    ASSERT_EQ("A Write Test" + WriteMessageStreamHandler::messageSuffix, result);
}
