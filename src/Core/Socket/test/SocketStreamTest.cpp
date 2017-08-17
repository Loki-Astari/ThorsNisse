#include "SocketStream.h"
#include <gtest/gtest.h>
#include <thread>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using ThorsAnvil::Socket::DataSocket;
using ThorsAnvil::Socket::ISocketStream;
using ThorsAnvil::Socket::OSocketStream;
using ReadInfo = std::pair<bool, std::size_t>;

TEST(SocketStreamTest, ReadNormal)
{
    int             socket  = open("test/data/SocketStreamTest-ReadNormal", O_RDONLY);
    DataSocket      dataSocket(socket);
    ISocketStream   stream(dataSocket);

    char data[16];
    stream.read(data,16);

    ASSERT_EQ(std::string(data, data + 16), std::string("1234567890ABCDEF"));
}
TEST(SocketStreamTest, ReadNormalButHugeChunk)
{
    int             socket  = open("test/data/SocketStreamTest-ReadLarge", O_RDONLY);
    DataSocket      dataSocket(socket);
    ISocketStream   stream(dataSocket);

    std::vector<char>   data(8000);
    stream.read(&data[0], 8000);
    EXPECT_EQ(8000, stream.gcount());
}
TEST(SocketStreamTest, MoveASocketStream)
{
    int             socket  = open("test/data/SocketStreamTest-ReadNormal", O_RDONLY);
    DataSocket      dataSocket(socket);
    ISocketStream   streamOriginal(dataSocket);
    ISocketStream   stream(std::move(streamOriginal));

    char data[16];
    stream.read(data,16);

    ASSERT_EQ(std::string(data, data + 16), std::string("1234567890ABCDEF"));
}
TEST(SocketStreamTest, ReadFromSlowStreamToGetEAGAIN)
{
    int sysres;
    int testData   = 5;
    int resultData = 0;

    int pipes[2];
    sysres = ::pipe(pipes);
    ASSERT_EQ(0, sysres);
    int flags = ::fcntl(pipes[0], F_GETFL, 0);
    ASSERT_NE(-1, flags);
    sysres = ::fcntl(pipes[0], F_SETFL, flags | O_NONBLOCK);
    ASSERT_NE(-1, sysres);

    std::thread slowStream([&testData, &pipes](){
        for(std::size_t loop=0;loop < sizeof(testData); ++loop) {
            sleep(1);
            ::write(pipes[1], reinterpret_cast<char const*>(&testData)+loop, 1);
        }
    });

    DataSocket      dataSocket(pipes[0], true);
    ISocketStream   stream(dataSocket);
    stream.read(reinterpret_cast<char*>(&resultData), sizeof(resultData));
    EXPECT_EQ(sizeof(resultData), stream.gcount());

    ASSERT_EQ(testData, resultData);
    ::close(pipes[1]);
    slowStream.join();
}
TEST(SocketStreamTest, ReadPastEOF)
{
    int             socket  = open("test/data/SocketStreamTest-ReadNormal", O_RDONLY);
    DataSocket      dataSocket(socket, true);
    ISocketStream   stream(dataSocket);

    char data[16];
    stream.read(data,16);
    EXPECT_EQ(16, stream.gcount());
    stream.read(data,16);
    EXPECT_EQ(1,  stream.gcount());
}

TEST(SocketStreamTest, ReadFail)
{
    int             socket  = open("test/data/SocketStreamTest-ReadNormal", O_RDONLY);
    DataSocket      dataSocket(socket, true);
    ISocketStream   stream(dataSocket);
    close(socket);

    char data[16];
    stream.read(data,16);
    EXPECT_EQ(0, stream.gcount());
}
TEST(SocketStreamTest, WriteNormal)
{
    int         socket  = open("test/data/SocketStreamTest-WriteNormal", O_WRONLY | O_CREAT | O_TRUNC, 0777 );
    {
        DataSocket      dataSocket(socket, true);
        OSocketStream   stream(dataSocket);

        char data[16] = "12345678";
        stream.write(data, 8);
    }
    {
        std::ifstream   test("test/data/SocketStreamTest-WriteNormal");
        std::string     line;
        std::getline(test, line);
        ASSERT_EQ("12345678", line);
    }
    unlink("test/data/SocketStreamTest-WriteNormal");
}
TEST(SocketStreamTest, WriteNormalWithMove)
{
    int         socket  = open("test/data/SocketStreamTest-WriteNormal", O_WRONLY | O_CREAT | O_TRUNC, 0777 );
    {
        DataSocket      dataSocket(socket, true);
        OSocketStream   streamOriginal(dataSocket);
        OSocketStream   stream(std::move(streamOriginal));

        char data[16] = "12345678";
        stream.write(data, 8);
    }
    {
        std::ifstream   test("test/data/SocketStreamTest-WriteNormal");
        std::string     line;
        std::getline(test, line);
        ASSERT_EQ("12345678", line);
    }
    unlink("test/data/SocketStreamTest-WriteNormal");
}
TEST(SocketStreamTest, WriteLarge)
{
    int         socket  = open("test/data/SocketStreamTest-WriteLarge", O_WRONLY | O_CREAT | O_TRUNC, 0777 );
    DataSocket      dataSocket(socket, true);
    OSocketStream   stream(dataSocket);

    std::vector<char> data(8000);
    stream.write(&data[0], 8000);
    unlink("test/data/SocketStreamTest-WriteLarge");
}
TEST(SocketStreamTest, WriteFail)
{
    int             socket  = open("test/data/SocketStreamTest-WriteNormal", O_WRONLY | O_CREAT | O_TRUNC, 0777 );
    DataSocket      dataSocket(socket, true);
    OSocketStream   stream(dataSocket);
    close(socket);

    char data[16] = "12345678";
    stream.write(data,8);
    unlink("test/data/SocketStreamTest-WriteNormal");
}
TEST(SocketStreamTest, WriteToSlowStreamToGetEAGAIN)
{
    int sysres;
    int const blocks   = 4;
    int const actCount = 46;
    int const bufSize  = 524288 / (blocks * actCount);
    std::vector<char> testData(bufSize);
    std::vector<char> resultData(bufSize);

    for(int loop=0; loop < bufSize; ++loop) {
        testData[loop] = ('0' + (loop % 10));
    }

    int pipes[2];
    sysres = ::pipe(pipes);
    ASSERT_EQ(0, sysres);
    int flags = ::fcntl(pipes[1], F_GETFL, 0);
    ASSERT_NE(-1, flags);
    sysres = ::fcntl(pipes[1], F_SETFL, flags | O_NONBLOCK);
    ASSERT_NE(-1, sysres);

    std::thread slowStream([&resultData, &pipes]() {
        for(int blockLoop = 0; blockLoop < blocks; ++blockLoop)
        {
            sleep(1);
            for(int loop=0;loop < actCount; ++loop)
            {
                int readSoFar = 0;
                while(readSoFar != bufSize)
                {
                    int nextRead = ::read(pipes[0], &resultData[readSoFar], bufSize - readSoFar);
                    if (nextRead == -1 && errno == EAGAIN)
                    {
                        continue;
                    }
                    ASSERT_NE(-1, nextRead);
                    readSoFar += nextRead;
                }
            }
        }
    });

    DataSocket      dataSocket(pipes[1], true);
    OSocketStream   stream(dataSocket);
    for(int blockLoop = 0; blockLoop < blocks; ++blockLoop)
    {
        for(int loop=0;loop < actCount; ++loop)
        {
            stream.write(&testData[0], bufSize);
        }
    }
    slowStream.join();

    for(int loop = 0; loop < bufSize; ++loop) {
        ASSERT_EQ(testData[loop], resultData[loop]);
    }
    ::close(pipes[0]);
}
