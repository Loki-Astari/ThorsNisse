#include "Socket.h"
#include <fstream>
#include <sys/socket.h>
#include <gtest/gtest.h>
#include <future>
#include <unistd.h>
#include <fcntl.h>

using ThorsAnvil::Socket::BaseSocket;
using ThorsAnvil::Socket::DataSocket;
using ThorsAnvil::Socket::ConnectSocket;
using ThorsAnvil::Socket::ServerSocket;
using ReadInfo = std::pair<bool, std::size_t>;

class DerivedFromBase: public BaseSocket
{
    public:
        DerivedFromBase()
            : BaseSocket()
        {}
        DerivedFromBase(int socketId, bool blocking = true)
            : BaseSocket(socketId, blocking)
        {}
};

TEST(SocketTest, defaultConstruct)
{
    DerivedFromBase     derived;
}
TEST(SocketTest, baseSocketInitFail)
{
    ASSERT_THROW(
        {DerivedFromBase     derived(-1);},
        std::domain_error
    );
}
TEST(SocketTest, baseSocketFailNonBlocking)
{
    auto doTest = [](){DerivedFromBase     derived(4, false);};

    ASSERT_THROW(
        {doTest();},
        std::domain_error
    );
}
TEST(SocketTest, baseSocketInitNonBlocking)
{
    int sock = ::socket(PF_INET, SOCK_STREAM, 0);
    DerivedFromBase     derived(sock, false);
}
TEST(SocketTest, baseSocketInitBlocking)
{
    int sock = ::socket(PF_INET, SOCK_STREAM, 0);
    DerivedFromBase     derived(sock, true);
}
TEST(SocketTest, baseSocketCloseInvalidSocket)
{
    int sock = ::socket(PF_INET, SOCK_STREAM, 0);
    DerivedFromBase     derived1(sock);
    DerivedFromBase     derived2(std::move(derived1));

    ASSERT_THROW(
        {derived1.close();},
        std::logic_error
    );
}
TEST(SocketTest, baseSocketCloseBadSocket)
{
    DerivedFromBase     derived1(3);

    ASSERT_THROW(
        {derived1.close();},
        std::domain_error
    );
}
TEST(SocketTest, baseSocketMoveConstruct)
{
    int sock = ::socket(PF_INET, SOCK_STREAM, 0);
    DerivedFromBase     derived1(sock);
    DerivedFromBase     derived2(std::move(derived1));

    EXPECT_EQ(-1,    derived1.getSocketId());
    EXPECT_EQ(sock,  derived2.getSocketId());
}
TEST(SocketTest, baseSocketMoveAssign)
{
    int sock1 = ::socket(PF_INET, SOCK_STREAM, 0);
    int sock2 = ::socket(PF_INET, SOCK_STREAM, 0);
    DerivedFromBase     derived1(sock1);
    DerivedFromBase     derived2(sock2);

    derived2 = std::move(derived1);

    EXPECT_EQ(sock1, derived2.getSocketId());
}
TEST(SocketTest, baseSocketSwap)
{
    int sock1 = ::socket(PF_INET, SOCK_STREAM, 0);
    int sock2 = ::socket(PF_INET, SOCK_STREAM, 0);
    DerivedFromBase     derived1(sock1);
    DerivedFromBase     derived2(sock2);

    using std::swap;
    swap(derived1, derived2);

    EXPECT_EQ(sock1,  derived2.getSocketId());
    EXPECT_EQ(sock2,  derived1.getSocketId());
}
TEST(SocketTest, ConnectSocketBadDomain)
{
    auto test = [](){ConnectSocket   socket("BadDomain.crap", 80);};
    ASSERT_THROW(
        {test();},
        std::runtime_error
    );
}
TEST(SocketTest, ConnectSocketBadPort)
{
    auto test = [](){ConnectSocket   socket("amazon.com", 45678);};
    ASSERT_THROW(
        {test();},
        std::domain_error
    );
}
TEST(SocketTest, ConnectSocket)
{
    ConnectSocket   socket("amazon.com", 80);
}
TEST(SocketTest, ServerSocketBadPort)
{
    ASSERT_THROW(
        {ServerSocket   socket(80);},
        std::runtime_error
    );
}
TEST(SocketTest, ServerSocketAcceptAfterClosed)
{
    ServerSocket   socket(12345678);
    socket.close();

    ASSERT_THROW(
        {socket.accept();},
        std::logic_error
    );
}
TEST(SocketTest, ServerSocketAccept)
{
    ServerSocket    socket(12345678);
    std::async([&socket](){ConnectSocket connect("127.0.0.1", 12345678);});
    DataSocket      connection = socket.accept();

    ASSERT_NE(-1, connection.getSocketId());
}
TEST(SocketTest, readOneLine)
{
    int fd[2];
    std::string const testData    = "A line of text\n";
    EXPECT_EQ(0, ::pipe(fd));
    EXPECT_EQ(testData.size(), ::write(fd[1], testData.c_str(), testData.size()));
    EXPECT_EQ(0, ::close(fd[1]));

    DataSocket      pipeReader(fd[0]);
    std::string     buffer(testData.size(), '\0');
    ReadInfo read = pipeReader.getMessageData(&buffer[0], testData.size());
    ASSERT_EQ(true, read.first);
    ASSERT_EQ(testData.size(), read.second);
    EXPECT_EQ(testData, buffer);
}
TEST(SocketTest, readFromAClosedSocket)
{
    int fd[2];
    std::string const testData    = "A line of text\n";
    EXPECT_EQ(0, ::pipe(fd));
    EXPECT_EQ(testData.size(), ::write(fd[1], testData.c_str(), testData.size()));
    EXPECT_EQ(0, ::close(fd[1]));

    DataSocket      pipeReader(fd[0]);
    pipeReader.close();
    std::string     buffer(testData.size(), '\0');
    ASSERT_THROW(
        {pipeReader.getMessageData(&buffer[0], testData.size());},
        std::logic_error
    );
}
TEST(SocketTest, readMoreDataThanIsAvailable)
{
    int fd[2];
    std::string const testData    = "A line of text\n";
    EXPECT_EQ(0, ::pipe(fd));
    EXPECT_EQ(testData.size(), ::write(fd[1], testData.c_str(), testData.size()));
    EXPECT_EQ(0, ::close(fd[1]));

    DataSocket      pipeReader(fd[0]);
    std::string     buffer(testData.size() + 10, '\0');
    ReadInfo read = pipeReader.getMessageData(&buffer[0], testData.size() + 10);
    EXPECT_EQ(false, read.first);
    EXPECT_EQ(testData.size(), read.second);
    buffer.resize(read.second);
    EXPECT_EQ(testData, buffer);
}
TEST(SocketTest, readMoreDataThanIsAvailableFromNonBlockingStream)
{
    int fd[2];
    std::string const testData    = "A line of text\n";
    EXPECT_EQ(0, ::pipe(fd));
    EXPECT_EQ(testData.size(), ::write(fd[1], testData.c_str(), testData.size()));

    DataSocket      pipeReader(fd[0]);
    std::string     buffer(testData.size() + 10, '\0');
    ReadInfo read = pipeReader.getMessageData(&buffer[0], testData.size() + 10);
    EXPECT_EQ(true, read.first);
    EXPECT_EQ(testData.size(), read.second);
    buffer.resize(read.second);
    EXPECT_EQ(testData, buffer);

    EXPECT_EQ(0, ::close(fd[1]));
    read = pipeReader.getMessageData(&buffer[0], testData.size() + 10);
    EXPECT_EQ(false, read.first);
    EXPECT_EQ(0, read.second);
}
TEST(SocketTest, readFromNullBuffer)
{
    int fd[2];
    std::string const testData    = "A line of text\n";
    EXPECT_EQ(0, ::pipe(fd));
    EXPECT_EQ(testData.size(), ::write(fd[1], testData.c_str(), testData.size()));
    EXPECT_EQ(0, ::close(fd[1]));

    DataSocket      pipeReader(fd[0]);
    std::string     buffer(testData.size(), '\0');

    auto doTest = [&pipeReader, &testData](){pipeReader.getMessageData(nullptr, testData.size());};
    ASSERT_THROW(
        {doTest();},
        std::domain_error
    );
}
TEST(SocketTest, writeOneLine)
{
    int fd[2];
    std::string const testData    = "A line of text\n";
    EXPECT_EQ(0, ::pipe(fd));

    DataSocket      pipeWriter(fd[1]);
    pipeWriter.putMessageData(testData.c_str(), testData.size());


    std::string     buffer(testData.size(), '\0');
    EXPECT_EQ(testData.size(), ::read(fd[0], &buffer[0], testData.size()));
    EXPECT_EQ(0, ::close(fd[0]));
    EXPECT_EQ(testData, buffer);
}
TEST(SocketTest, writeToAClosedSocket)
{
    int fd[2];
    std::string const testData    = "A line of text\n";
    EXPECT_EQ(0, ::pipe(fd));
    EXPECT_EQ(0, ::close(fd[0]));

    DataSocket      pipeWriter(fd[1]);
    pipeWriter.close();

    auto doTest = [&pipeWriter, &testData](){pipeWriter.putMessageData(testData.c_str(), testData.size());};
    ASSERT_THROW(
        {doTest();},
        std::logic_error
    );
}
TEST(SocketTest, MessageCloseFailBecauseItsNotASocket)
{
    int fd[2];
    EXPECT_EQ(0, ::pipe(fd));
    EXPECT_EQ(0, ::close(fd[0]));

    DataSocket      pipeWriter(fd[1]);
    ASSERT_THROW(
        {pipeWriter.putMessageClose();},
        std::domain_error
    );
}

