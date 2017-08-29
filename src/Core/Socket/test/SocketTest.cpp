#include "Socket.h"
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
    auto future = std::async( std::launch::async, [&socket](){ConnectSocket connect("127.0.0.1", 12345678);});
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

TEST(SocketExceptionTest, baseSocket_InitFail)
{
    ASSERT_THROW(
        {DerivedFromBase     derived(-1);},
        std::domain_error
    );
}
TEST(SocketExceptionTest, baseSocketFailNonBlocking)
{
    MOCK_SYS(fcntlWrapper, [](int, int, int){return -1;});
    auto doTest = [](){DerivedFromBase     derived(4, false);};

    ASSERT_THROW(
        {doTest();},
        std::domain_error
    );
}

TEST(SocketExceptionTest, baseSocketCloseInvalidSocket)
{
    DerivedFromBase     derived1(5);
    DerivedFromBase     derived2(std::move(derived1));

    ASSERT_THROW(
        {derived1.close();},
        std::logic_error
    );
}

TEST(SocketExceptionTest, CloseFail_EBADF)
{
    MOCK_SYS(closeWrapper, [](int socketId){errno = EBADF;return -1;});

    DerivedFromBase  socket(5);
    ASSERT_THROW(
        socket.close(),
        std::domain_error
    );
}

TEST(SocketExceptionTest, CloseFail_EIO)
{
    MOCK_SYS(closeWrapper, [](int socketId){errno = EIO;return -1;});

    DerivedFromBase  socket(5);
    ASSERT_THROW(
        socket.close(),
        std::runtime_error
    );
}
TEST(SocketExceptionTest, CloseFail_EINTR)
{
    int count = 0;
    MOCK_SYS(closeWrapper, [&count](int socketId){
        ++count;
        if (count == 1) {errno = EINTR;return -1;}
        return 0;
    });

    DerivedFromBase  socket(5);
    socket.close();

    ASSERT_EQ(count, 2);
    ASSERT_EQ(socket.getSocketId(), -1);
}
TEST(SocketExceptionTest, CloseFail_Unknown)
{
    MOCK_SYS(closeWrapper, [](int socketId){errno = 9998;return -1;});

    DerivedFromBase  socket(5);
    ASSERT_THROW(
        socket.close(),
        std::domain_error
    );
}
TEST(SocketExceptionTest, ConnectionSocketFailsToOpenSocket)
{
    MOCK_SYS(socketWrapper, [](int, int, int){return -1;});
    auto doTest = [](){ConnectSocket   connect("thorsanvil.com", 80);};

    ASSERT_THROW(
        doTest(),
        std::domain_error
    );
}
TEST(SocketExceptionTest, ConnectionSocketFailsToGetHostByName)
{
    MOCK_SYS(socketWrapper, [](int, int, int){return 5;});
    MOCK_SYS(gethostbyname, [](char const*){return nullptr;});
    auto doTest = [](){ConnectSocket   connect("thorsanvil.com", 80);};

    ASSERT_THROW(
        doTest(),
        std::runtime_error
    );
}
TEST(SocketExceptionTest, ConnectionSocketFailsToGetHostByNameTryAgain)
{
    int count = 0;
    MOCK_SYS(socketWrapper, [](int, int, int){return 5;});
    MOCK_SYS(gethostbyname, [&count](char const*){
        ++count;
        if (count == 1) {h_errno = TRY_AGAIN;return nullptr;}
        h_errno = 0;
        return nullptr;
    });
    auto doTest = [](){ConnectSocket   connect("thorsanvil.com", 80);};

    ASSERT_THROW(
        doTest(),
        std::runtime_error
    );
    ASSERT_EQ(count, 2);
}
TEST(SocketExceptionTest, ConnectionSocketFailsConnect)
{
    MOCK_SYS(socketWrapper, [](int, int, int){return 5;});
    MOCK_SYS(gethostbyname, [](char const*){static char buf[5];static char* bufH[1];static HostEnt result;bufH[0]=buf;result.h_addr_list=bufH;result.h_length=0;return &result;});
    MOCK_SYS(connect,       [](int, SocketAddr const*, std::size_t){return -1;});
    MOCK_SYS(closeWrapper,  [](int socketId){return 0;});
    auto doTest = [](){ConnectSocket   connect("thorsanvil.com", 80);};

    ASSERT_THROW(
        doTest(),
        std::domain_error
    );
}
TEST(SocketExceptionTest, ServerSocketFailsToOpenSocket)
{
    MOCK_SYS(socketWrapper, [](int, int, int){return -1;});
    auto doTest = [](){ServerSocket   server(8080);};

    ASSERT_THROW(
        doTest(),
        std::domain_error
    );
}
TEST(SocketExceptionTest, ServerSocketFailsToBind)
{
    MOCK_SYS(socketWrapper, [](int, int, int){return 5;});
    MOCK_SYS(bind,          [](int, SocketAddr const*, std::size_t){return -1;});
    MOCK_SYS(closeWrapper,  [](int socketId){return 0;});

    auto doTest = [](){ServerSocket   server(8080, true);};

    ASSERT_THROW(
        doTest(),
        std::runtime_error
    );
}
TEST(SocketExceptionTest, ServerSocketFailsToListen)
{
    MOCK_SYS(socketWrapper, [](int, int, int){return 5;});
    MOCK_SYS(bind,          [](int, SocketAddr const*, std::size_t){return 0;});
    MOCK_SYS(listen,        [](int, int){return -1;});
    MOCK_SYS(closeWrapper,  [](int socketId){return 0;});

    auto doTest = [](){ServerSocket   server(8080, true);};

    ASSERT_THROW(
        doTest(),
        std::runtime_error
    );
}
TEST(SocketExceptionTest, ServerSocketAcceptFailsInvalidId)
{
    MOCK_SYS(socketWrapper, [](int, int, int){return 5;});
    MOCK_SYS(bind,          [](int, SocketAddr const*, std::size_t){return 0;});
    MOCK_SYS(listen,        [](int, int){return 0;});
    MOCK_SYS(closeWrapper,  [](int socketId){return 0;});

    ServerSocket    server1(8080, true);
    ServerSocket    server2(std::move(server1));
    ASSERT_THROW(
        server1.accept(),
        std::logic_error
    );
}
TEST(SocketExceptionTest, ServerSocketAcceptFailsAcceptCall)
{
    MOCK_SYS(socketWrapper, [](int, int, int){return 5;});
    MOCK_SYS(bind,          [](int, SocketAddr const*, std::size_t){return 0;});
    MOCK_SYS(listen,        [](int, int){return 0;});
    MOCK_SYS(closeWrapper,  [](int socketId){return 0;});
    MOCK_SYS(acceptWrapper, [](int, SocketAddr*, socklen_t*){return -1;});

    ServerSocket    server(8080, true);
    ASSERT_THROW(
        server.accept(),
        std::runtime_error
    );
}
TEST(SocketExceptionTest, DataSocketaReadInvalidId)
{
    DataSocket          data1(5, true);
    DataSocket          data2(std::move(data1));

    auto doTest = [](DataSocket& data1){data1.getMessageData(nullptr, 0, 0);};

    ASSERT_THROW(
        doTest(data1),
        std::logic_error
    );
}
TEST(SocketExceptionTest, DataSocketaReadFailsEBADFOnRead)
{
    MOCK_SYS(readWrapper,   [](int, void*, std::size_t){errno = EBADF;return -1;});
    DataSocket          data(5, true);

    auto doTest = [](DataSocket& data){data.getMessageData(nullptr, 5, 0);};

    ASSERT_THROW(
        doTest(data),
        std::domain_error
    );
}
TEST(SocketExceptionTest, DataSocketaReadFailsEFAULTOnRead)
{
    MOCK_SYS(readWrapper,   [](int, void*, std::size_t){errno = EFAULT;return -1;});
    DataSocket          data(5, true);

    auto doTest = [](DataSocket& data){data.getMessageData(nullptr, 5, 0);};

    ASSERT_THROW(
        doTest(data),
        std::domain_error
    );
}
TEST(SocketExceptionTest, DataSocketaReadFailsEINVALOnRead)
{
    MOCK_SYS(readWrapper,   [](int, void*, std::size_t){errno = EINVAL;return -1;});
    DataSocket          data(5, true);

    auto doTest = [](DataSocket& data){data.getMessageData(nullptr, 5, 0);};

    ASSERT_THROW(
        doTest(data),
        std::domain_error
    );
}
TEST(SocketExceptionTest, DataSocketaReadFailsENXIOOnRead)
{
    MOCK_SYS(readWrapper,   [](int, void*, std::size_t){errno = ENXIO;return -1;});
    DataSocket          data(5, true);

    auto doTest = [](DataSocket& data){data.getMessageData(nullptr, 5, 0);};

    ASSERT_THROW(
        doTest(data),
        std::domain_error
    );
}
TEST(SocketExceptionTest, DataSocketaReadFailsENOMEMOnRead)
{
    MOCK_SYS(readWrapper,   [](int, void*, std::size_t){errno = ENOMEM;return -1;});
    DataSocket          data(5, true);

    auto doTest = [](DataSocket& data){data.getMessageData(nullptr, 5, 0);};

    ASSERT_THROW(
        doTest(data),
        std::domain_error
    );
}
TEST(SocketExceptionTest, DataSocketaReadFailsEIOOnRead)
{
    MOCK_SYS(readWrapper,   [](int, void*, std::size_t){errno = EIO;return -1;});
    DataSocket          data(5, true);

    auto doTest = [](DataSocket& data){data.getMessageData(nullptr, 5, 0);};

    ASSERT_THROW(
        doTest(data),
        std::runtime_error
    );
}
TEST(SocketExceptionTest, DataSocketaReadFailsENOBUFSOnRead)
{
    MOCK_SYS(readWrapper,   [](int, void*, std::size_t){errno = ENOBUFS;return -1;});
    DataSocket          data(5, true);

    auto doTest = [](DataSocket& data){data.getMessageData(nullptr, 5, 0);};

    ASSERT_THROW(
        doTest(data),
        std::runtime_error
    );
}
TEST(SocketExceptionTest, DataSocketaReadFailsUnknownOnRead)
{
    MOCK_SYS(readWrapper,   [](int, void*, std::size_t){errno = 9998;return -1;});
    DataSocket          data(5, true);

    auto doTest = [](DataSocket& data){data.getMessageData(nullptr, 5, 0);};

    ASSERT_THROW(
        doTest(data),
        std::runtime_error
    );
}
TEST(SocketExceptionTest, DataSocketaReadFailsEINTROnRead)
{
    int count = 0;
    MOCK_SYS(readWrapper,   [&count](int, void*, std::size_t){
        ++count;
        if (count == 1){errno = EINTR;return -1;}
        errno = EIO;
        return -1;
    });
    DataSocket          data(5, true);

    auto doTest = [](DataSocket& data){data.getMessageData(nullptr, 5, 0);};

    ASSERT_THROW(
        doTest(data),
        std::runtime_error
    );
    ASSERT_EQ(count, 2);
}
TEST(SocketExceptionTest, DataSocketaReadFailsETIMEDOUTOnRead)
{
    MOCK_SYS(readWrapper,   [](int, void*, std::size_t){errno = ETIMEDOUT;return -1;});
    DataSocket          data(5, true);

    auto result = data.getMessageData(nullptr, 5, 0);
    ASSERT_TRUE(result.first);
    ASSERT_EQ(result.second, 0);
}
TEST(SocketExceptionTest, DataSocketaReadFailsEAGAINOnRead)
{
    MOCK_SYS(readWrapper,   [](int, void*, std::size_t){errno = EAGAIN;return -1;});
    DataSocket          data(5, true);

    auto result = data.getMessageData(nullptr, 5, 0);
    ASSERT_TRUE(result.first);
    ASSERT_EQ(result.second, 0);
}
TEST(SocketExceptionTest, DataSocketaReadFailsECONNRESETOnRead)
{
    MOCK_SYS(readWrapper,   [](int, void*, std::size_t){errno = ECONNRESET;return -1;});
    DataSocket          data(5, true);

    auto result = data.getMessageData(nullptr, 5, 0);
    ASSERT_FALSE(result.first);
    ASSERT_EQ(result.second, 0);
}
TEST(SocketExceptionTest, DataSocketaReadFailsENOTCONNOnRead)
{
    MOCK_SYS(readWrapper,   [](int, void*, std::size_t){errno = ENOTCONN;return -1;});
    DataSocket          data(5, true);

    auto result = data.getMessageData(nullptr, 5, 0);
    ASSERT_FALSE(result.first);
    ASSERT_EQ(result.second, 0);
}
TEST(SocketExceptionTest, DataSocketaWriteInvalidId)
{
    DataSocket          data1(5, true);
    DataSocket          data2(std::move(data1));

    auto doTest = [](DataSocket& data1){data1.putMessageData(nullptr, 0, 0);};

    ASSERT_THROW(
        doTest(data1),
        std::logic_error
    );
}
TEST(SocketExceptionTest, DataSocketaWriteFailsEINVALOnRead)
{
    MOCK_SYS(writeWrapper,  [](int, void const*, std::size_t){errno = EINVAL;return -1;});
    DataSocket          data(5, true);

    auto doTest = [](DataSocket& data){data.putMessageData(nullptr, 5, 0);};

    ASSERT_THROW(
        doTest(data),
        std::domain_error
    );
}
TEST(SocketExceptionTest, DataSocketaWriteFailsEBADFOnRead)
{
    MOCK_SYS(writeWrapper,  [](int, void const*, std::size_t){errno = EBADF;return -1;});
    DataSocket          data(5, true);

    auto doTest = [](DataSocket& data){data.putMessageData(nullptr, 5, 0);};

    ASSERT_THROW(
        doTest(data),
        std::domain_error
    );
}
TEST(SocketExceptionTest, DataSocketaWriteFailsECONNRESETOnRead)
{
    MOCK_SYS(writeWrapper,  [](int, void const*, std::size_t){errno = ECONNRESET;return -1;});
    DataSocket          data(5, true);

    auto doTest = [](DataSocket& data){data.putMessageData(nullptr, 5, 0);};

    ASSERT_THROW(
        doTest(data),
        std::domain_error
    );
}
TEST(SocketExceptionTest, DataSocketaWriteFailsENXIOOnRead)
{
    MOCK_SYS(writeWrapper,  [](int, void const*, std::size_t){errno = ENXIO;return -1;});
    DataSocket          data(5, true);

    auto doTest = [](DataSocket& data){data.putMessageData(nullptr, 5, 0);};

    ASSERT_THROW(
        doTest(data),
        std::domain_error
    );
}
TEST(SocketExceptionTest, DataSocketaWriteFailsEPIPEOnRead)
{
    MOCK_SYS(writeWrapper,  [](int, void const*, std::size_t){errno = EPIPE;return -1;});
    DataSocket          data(5, true);

    auto doTest = [](DataSocket& data){data.putMessageData(nullptr, 5, 0);};

    ASSERT_THROW(
        doTest(data),
        std::domain_error
    );
}
TEST(SocketExceptionTest, DataSocketaWriteFailsEDQUOTOnRead)
{
    MOCK_SYS(writeWrapper,  [](int, void const*, std::size_t){errno = EDQUOT;return -1;});
    DataSocket          data(5, true);

    auto doTest = [](DataSocket& data){data.putMessageData(nullptr, 5, 0);};

    ASSERT_THROW(
        doTest(data),
        std::runtime_error
    );
}
TEST(SocketExceptionTest, DataSocketaWriteFailsEFBIGnRead)
{
    MOCK_SYS(writeWrapper,  [](int, void const*, std::size_t){errno = EFBIG;return -1;});
    DataSocket          data(5, true);

    auto doTest = [](DataSocket& data){data.putMessageData(nullptr, 5, 0);};

    ASSERT_THROW(
        doTest(data),
        std::runtime_error
    );
}
TEST(SocketExceptionTest, DataSocketaWriteFailsEIOOnRead)
{
    MOCK_SYS(writeWrapper,  [](int, void const*, std::size_t){errno = EIO;return -1;});
    DataSocket          data(5, true);

    auto doTest = [](DataSocket& data){data.putMessageData(nullptr, 5, 0);};

    ASSERT_THROW(
        doTest(data),
        std::runtime_error
    );
}
TEST(SocketExceptionTest, DataSocketaWriteFailsENETDOWNOnRead)
{
    MOCK_SYS(writeWrapper,  [](int, void const*, std::size_t){errno = ENETDOWN;return -1;});
    DataSocket          data(5, true);

    auto doTest = [](DataSocket& data){data.putMessageData(nullptr, 5, 0);};

    ASSERT_THROW(
        doTest(data),
        std::runtime_error
    );
}
TEST(SocketExceptionTest, DataSocketaWriteFailsENETUNREACHOnRead)
{
    MOCK_SYS(writeWrapper,  [](int, void const*, std::size_t){errno = ENETUNREACH;return -1;});
    DataSocket          data(5, true);

    auto doTest = [](DataSocket& data){data.putMessageData(nullptr, 5, 0);};

    ASSERT_THROW(
        doTest(data),
        std::runtime_error
    );
}
TEST(SocketExceptionTest, DataSocketaWriteFailsENOSPCOnRead)
{
    MOCK_SYS(writeWrapper,  [](int, void const*, std::size_t){errno = ENOSPC;return -1;});
    DataSocket          data(5, true);

    auto doTest = [](DataSocket& data){data.putMessageData(nullptr, 5, 0);};

    ASSERT_THROW(
        doTest(data),
        std::runtime_error
    );
}
TEST(SocketExceptionTest, DataSocketaWriteFailsUnknownOnRead)
{
    MOCK_SYS(writeWrapper,  [](int, void const*, std::size_t){errno = 9998;return -1;});
    DataSocket          data(5, true);

    auto doTest = [](DataSocket& data){data.putMessageData(nullptr, 5, 0);};

    ASSERT_THROW(
        doTest(data),
        std::runtime_error
    );
}
TEST(SocketExceptionTest, DataSocketaPutMessageCloseFails)
{
    MOCK_SYS(shutdown,      [](int, int){errno = ENOSPC;return -1;});
    DataSocket          data(5, true);

    ASSERT_THROW(
        data.putMessageClose(),
        std::domain_error
    );
}
TEST(SocketExceptionTest, DataSocketaWriteFailsEINTROnWrite)
{
    int count = 0;
    MOCK_SYS(writeWrapper,  [&count](int, void const*, std::size_t){
        ++count;
        if (count == 1){errno = EINTR;return -1;}
        errno = EIO;
        return -1;
    });
    DataSocket          data(5, true);

    auto doTest = [](DataSocket& data){data.putMessageData(nullptr, 5, 0);};

    ASSERT_THROW(
        doTest(data),
        std::runtime_error
    );
    ASSERT_EQ(count, 2);
}
TEST(SocketExceptionTest, DataSocketaWriteFailsETIMEDOUTOnWrite)
{
    MOCK_SYS(writeWrapper,  [](int, void const*, std::size_t){errno = ETIMEDOUT;return -1;});
    DataSocket          data(5, true);

    auto result = data.putMessageData(nullptr, 5, 0);
    ASSERT_TRUE(result.first);
    ASSERT_EQ(result.second, 0);
}
TEST(SocketExceptionTest, DataSocketaWriteFailsEAGAINOnWrite)
{
    MOCK_SYS(writeWrapper,  [](int, void const*, std::size_t){errno = EAGAIN;return -1;});
    DataSocket          data(5, true);

    auto result = data.putMessageData(nullptr, 5, 0);
    ASSERT_TRUE(result.first);
    ASSERT_EQ(result.second, 0);
}
