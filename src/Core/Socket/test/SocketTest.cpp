#include "Socket.h"
#include "SocketWrapper.h"
#include "coverage/ThorMock.h"
#include <fstream>
#include <sys/socket.h>
#include <gtest/gtest.h>
#include <future>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>

using ThorsAnvil::Nisse::Core::Socket::BaseSocket;
using ThorsAnvil::Nisse::Core::Socket::DataSocket;
using ThorsAnvil::Nisse::Core::Socket::ConnectSocket;
using ThorsAnvil::Nisse::Core::Socket::ServerSocket;
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
TEST(SocketTest, ConnectSocket)
{
    ConnectSocket   socket("amazon.com", 80);
}
TEST(SocketTest, ServerSocketAccept)
{
    ServerSocket    socket(12345678, true);
    auto future = std::async( std::launch::async, [](){ConnectSocket connect("127.0.0.1", 12345678);});
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

