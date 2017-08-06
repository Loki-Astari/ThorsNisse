#include "ProtocolSimple.h"
#include "ThorsNisse/NisseService.h"
#include "ThorsNisseSocket/Socket.h"
#include <gtest/gtest.h>
#include <future>

using ThorsAnvil::Nisse::NisseService;
using ThorsAnvil::Nisse::ProtocolSimple::ReadMessageHandler;
using ThorsAnvil::Nisse::ProtocolSimple::WriteMessageHandler;
using ThorsAnvil::Socket::ConnectSocket;

TEST(ProtocolSimpleTest, ReadMessageHandler)
{
    NisseService    service;
    bool            finished = false;

    service.listenOn<ReadMessageHandler>(9878);
    service.addTimer(1, [&service, &finished]()
    {
        if (finished)
        {
            service.flagShutDown();
        }
    });
        
    auto future = std::async([&finished]()
    {
        ConnectSocket connect("127.0.0.1", 9878);
        std::string message = "Test function ReadMessageHandler";
        std::size_t size    = message.size();
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

    ASSERT_EQ("Test function ReadMessageHandler" + WriteMessageHandler::messageSuffix, result);
}
TEST(ProtocolSimpleTest, ReadMessageHandlerPartialSize)
{
    NisseService    service;
    bool            finished = false;

    service.listenOn<ReadMessageHandler>(9879);
    service.addTimer(1, [&service, &finished]()
    {
        if (finished)
        {
            service.flagShutDown();
        }
    });
        
    auto future = std::async([&finished]()
    {
        ConnectSocket connect("127.0.0.1", 9879);
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

    service.start(1);
    std::string result = future.get();

    ASSERT_EQ(ReadMessageHandler::failSizeMessage, result);
}
TEST(ProtocolSimpleTest, ReadMessageHandlerPartialSizeInTwoChunks)
{
    NisseService    service;
    bool            finished = false;

    service.listenOn<ReadMessageHandler>(9880);
    service.addTimer(1, [&service, &finished]()
    {
        if (finished)
        {
            service.flagShutDown();
        }
    });
        
    auto future = std::async([&finished]()
    {
        ConnectSocket connect("127.0.0.1", 9880);
        std::string message = "Test function ReadMessageHandlerPartialSizeInTwoChunks";
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

    ASSERT_EQ(ReadMessageHandler::failSizeMessage, result);
}
TEST(ProtocolSimpleTest, ReadMessageHandlerSizeInTwoChunks)
{
    NisseService    service;
    bool            finished = false;

    service.listenOn<ReadMessageHandler>(9881);
    service.addTimer(1, [&service, &finished]()
    {
        if (finished)
        {
            service.flagShutDown();
        }
    });
        
    auto future = std::async([&finished]()
    {
        ConnectSocket connect("127.0.0.1", 9881);
        std::string message = "Test function ReadMessageHandlerSizeInTwoChunks";
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

    ASSERT_EQ("Test function ReadMessageHandlerSizeInTwoChunks" + WriteMessageHandler::messageSuffix, result);
}
TEST(ProtocolSimpleTest, ReadMessageHandlerPartialMessage)
{
    NisseService    service;
    bool            finished = false;

    service.listenOn<ReadMessageHandler>(9882);
    service.addTimer(1, [&service, &finished]()
    {
        if (finished)
        {
            service.flagShutDown();
        }
    });
        
    auto future = std::async([&finished]()
    {
        ConnectSocket connect("127.0.0.1", 9882);
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

    service.start(1);
    std::string result = future.get();

    ASSERT_EQ(ReadMessageHandler::failIncompleteMessage, result);
}
TEST(ProtocolSimpleTest, ReadMessageHandlerPartialMessageInTwoChunks)
{
    NisseService    service;
    bool            finished = false;

    service.listenOn<ReadMessageHandler>(9883);
    service.addTimer(1, [&service, &finished]()
    {
        if (finished)
        {
            service.flagShutDown();
        }
    });
        
    auto future = std::async([&finished]()
    {
        ConnectSocket connect("127.0.0.1", 9883);
        std::string message = "Test Function ReadMessageHandlerPartialMessageInTwoChunks";
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

    ASSERT_EQ(ReadMessageHandler::failIncompleteMessage, result);
}
TEST(ProtocolSimpleTest, ReadMessageHandlerMessageInTwoChunks)
{
    NisseService    service;
    bool            finished = false;

    service.listenOn<ReadMessageHandler>(9884);
    service.addTimer(1, [&service, &finished]()
    {
        if (finished)
        {
            service.flagShutDown();
        }
    });
        
    auto future = std::async([&finished]()
    {
        ConnectSocket connect("127.0.0.1", 9884);
        std::string message = "Test function ReadMessageHandlerMessageInTwoChunks";
        std::size_t size    = message.size();
        connect.putMessageData(reinterpret_cast<char*>(&size), sizeof(size));
        connect.putMessageData(message.c_str(), size/2);
        sleep(1);
        connect.putMessageData(message.c_str(), size, size/2);

        connect.getMessageData(reinterpret_cast<char*>(&size), sizeof(size));
        message.resize(size);
        connect.getMessageData(&message[0], size);

        finished = true;
        return message;
    });

    service.start(1);
    std::string result = future.get();

    ASSERT_EQ("Test function ReadMessageHandlerMessageInTwoChunks" + WriteMessageHandler::messageSuffix, result);
}
TEST(ProtocolSimpleTest, WriteMessageHandler)
{
    NisseService    service;
    bool            finished = false;
    std::string     message = "A Write Test";

    service.listenOn<WriteMessageHandler>(9885, message);

    service.addTimer(1, [&service, &finished]()
    {
        if (finished)
        {
            service.flagShutDown();
        }
    });
        
    auto future = std::async([&finished]()
    {
        ConnectSocket connect("127.0.0.1", 9885);
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

    ASSERT_EQ("A Write Test", result);
}
