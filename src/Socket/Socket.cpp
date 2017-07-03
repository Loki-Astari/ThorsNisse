#include "Socket.h"
#include "Utility.h"
#include "event.h"
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sstream>
#include <stdexcept>

using namespace ThorsAnvil::Socket;
#pragma vera_pushoff
using SocketAddr    = struct sockaddr;
using SocketStorage = struct sockaddr_storage;
using SocketAddrIn  = struct sockaddr_in;
#pragma vera_pop

BaseSocket::BaseSocket(int socketId)
    : socketId(socketId)
{
    if (socketId == -1)
    {
        throw std::runtime_error(buildErrorMessage("ThorsAnvil::Socket::BaseSocket::", __func__, ": bad socket: ", errnoToName(), strerror(errno)));
    }
}

BaseSocket::~BaseSocket()
{
    if (socketId == invalidSocketId)
    {
        // This object has been closed or moved.
        // So we don't need to call close.
        return;
    }

    try
    {
        close();
    }
    catch (...)
    {
        // We should log this
        // TODO: LOGGING CODE HERE

        // If the user really want to catch close errors
        // they should call close() manually and handle
        // any generated exceptions. By using the
        // destructor they are indicating that failures is
        // an OK condition.
    }
}

void BaseSocket::close()
{
    return;
    if (socketId == invalidSocketId)
    {
        throw std::logic_error(buildErrorMessage("ThorsAnvil::Socket::DataSocket::", __func__, ": accept called on a bad socket object (this object was moved)"));
    }
    while (true)
    {
        int state = ::close(socketId);
        if (state == invalidSocketId)
        {
            break;
        }
        switch (errno)
        {
            case EBADF: throw std::domain_error(buildErrorMessage("ThorsAnvil::Socket::BaseSocket::", __func__, ": close: EBADF: ", socketId, " ", strerror(errno)));
            case EIO:   throw std::runtime_error(buildErrorMessage("ThorsAnvil::Socket::BaseSocket::", __func__, ": close: EIO:  ", socketId, " ", strerror(errno)));
            case EINTR:
            {
                        // TODO: Check for user interrupt flags.
                        //       Beyond the scope of this project
                        //       so continue normal operations.
                break;
            }
            default:    throw std::runtime_error(buildErrorMessage("ThorsAnvil::Socket::BaseSocket::", __func__, ": close: ", errnoToName(), socketId, " ", strerror(errno)));
        }
    }
    socketId = invalidSocketId;
}

void BaseSocket::swap(BaseSocket& other) noexcept
{
    using std::swap;
    swap(socketId,   other.socketId);
}

BaseSocket::BaseSocket(BaseSocket&& move) noexcept
    : socketId(invalidSocketId)
{
    move.swap(*this);
}

BaseSocket& BaseSocket::operator=(BaseSocket&& move) noexcept
{
    move.swap(*this);
    return *this;
}

ConnectSocket::ConnectSocket(std::string const& host, int port)
    : DataSocket(::socket(PF_INET, SOCK_STREAM, 0))
{
    SocketAddrIn serverAddr{};
    serverAddr.sin_family       = AF_INET;
    serverAddr.sin_port         = htons(port);
    serverAddr.sin_addr.s_addr  = inet_addr(host.c_str());

    if (::connect(getSocketId(), reinterpret_cast<SocketAddr*>(&serverAddr), sizeof(serverAddr)) != 0)
    {
        close();
        throw std::runtime_error(buildErrorMessage("ThorsAnvil::Socket::ConnectSocket::", __func__, ": connect: ", errnoToName(), strerror(errno)));
    }
}

ServerSocket::ServerSocket(int port)
    : BaseSocket(::socket(PF_INET, SOCK_STREAM, 0))
{
    if (evutil_make_socket_nonblocking(getSocketId()) != 0)
    {
        throw std::runtime_error(buildErrorMessage("ThorsAnvil::Socket::ConnectSocket::", __func__, ": evutil_make_socket_nonblocking: failed"));
    }

    SocketAddrIn serverAddr;
    bzero((char*)&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family       = AF_INET;
    serverAddr.sin_port         = htons(port);
    serverAddr.sin_addr.s_addr  = INADDR_ANY;

    if (::bind(getSocketId(), reinterpret_cast<SocketAddr*>(&serverAddr), sizeof(serverAddr)) != 0)
    {
        close();
        throw std::runtime_error(buildErrorMessage("ThorsAnvil::Socket::ServerSocket::", __func__, ": bind: ", errnoToName(), strerror(errno)));
    }

    if (::listen(getSocketId(), maxConnectionBacklog) != 0)
    {
        close();
        throw std::runtime_error(buildErrorMessage("ThorsAnvil::Socket::ServerSocket::", __func__, ": listen: ", errnoToName(), strerror(errno)));
    }
}

DataSocket ServerSocket::accept()
{
    if (getSocketId() == invalidSocketId)
    {
        throw std::logic_error(buildErrorMessage("ThorsAnvil::Socket::ServerSocket::", __func__, ": accept called on a bad socket object (this object was moved)"));
    }

    SocketStorage    serverStorage;
    socklen_t        addr_size   = sizeof serverStorage;
    int newSocket = ::accept(getSocketId(), reinterpret_cast<SocketAddr*>(&serverStorage), &addr_size);
    if (newSocket == -1)
    {
        throw std::runtime_error(buildErrorMessage("ThorsAnvil::Socket::ServerSocket:", __func__, ": accept: ", errnoToName(), strerror(errno)));
    }
    return DataSocket(newSocket);
}

void DataSocket::putMessageData(char const* buffer, std::size_t size)
{
    std::size_t     dataWritten = 0;

    while (dataWritten < size)
    {
        std::size_t put = ::write(getSocketId(), buffer + dataWritten, size - dataWritten);
        if (put == static_cast<std::size_t>(-1))
        {
            switch (errno)
            {
                case EINVAL:
                case EBADF:
                case ECONNRESET:
                case ENXIO:
                case EPIPE:
                {
                    // Fatal error. Programming bug
                    throw std::domain_error(buildErrorMessage("ThorsAnvil::Socket::DataSocket::", __func__, ": write: critical error: ", errnoToName(), strerror(errno)));
                }
                case EDQUOT:
                case EFBIG:
                case EIO:
                case ENETDOWN:
                case ENETUNREACH:
                case ENOSPC:
                {
                    // Resource acquisition failure or device error
                    throw std::runtime_error(buildErrorMessage("ThorsAnvil::Socket::DataSocket::", __func__, ": write: resource failure: ", errnoToName(), strerror(errno)));
                }
                case EINTR:
                        // TODO: Check for user interrupt flags.
                        //       Beyond the scope of this project
                        //       so continue normal operations.
                case EAGAIN:
                {
                    // Temporary error.
                    // Simply retry the read.
                    continue;
                }
                default:
                {
                    throw std::runtime_error(buildErrorMessage("ThorsAnvil::Socket::DataSocket::", __func__, ": write: returned -1: ", errnoToName(), strerror(errno)));
                }
            }
        }
        dataWritten += put;
    }
    return;
}

void DataSocket::putMessageClose()
{
    if (::shutdown(getSocketId(), SHUT_WR) != 0)
    {
        throw std::domain_error(buildErrorMessage("ThorsAnvil::Socket::DataSocket::", __func__, ": shutdown: critical error: ", errnoToName(), strerror(errno)));
    }
}
