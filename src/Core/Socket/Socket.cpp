#include "Socket.h"
#include "ThorsNisseCoreUtility/Utility.h"
#include <stdexcept>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>

using namespace ThorsAnvil::Nisse::Core::Socket;

BaseSocket::BaseSocket()
    : socketId(invalidSocketId)
{}

Detail::SocketInterface sys
{
    [](int socketId, int cmd, int arg)                    {return ::fcntl(socketId, cmd, arg);},
    [](int socketId)                                      {return ::close(socketId);},
    [](int domain, int type, int protocol)                {return ::socket(domain, type, protocol);},
    [](int socketId, SocketAddr* addr, std::size_t size)  {return ::connect(socketId, addr, size);},
    [](int socketId, SocketAddr* addr, std::size_t size)  {return ::bind(socketId, addr, size);},
    [](int socketId, int max)                             {return ::listen(socketId, max);},
    [](int socketId, SocketAddr* addr, socklen_t* size)   {return ::accept(socketId, addr, size);},
    [](int socketId, void* buffer, std::size_t size)      {return ::read(socketId, buffer, size);},
    [](int socketId, void const* buffer, std::size_t size){return ::write(socketId, buffer, size);},
    [](int socketId, int how)                             {return ::shutdown(socketId, how);},
    [](char const* host)                                  {return ::gethostbyname(host);}
};

BaseSocket::BaseSocket(int socketId, bool blocking)
    : socketId(socketId)
{
    if (socketId == invalidSocketId)
    {
        throw std::domain_error(Utility::buildErrorMessage("ThorsAnvil::Socket::BaseSocket::", __func__,
                                                  ": bad socket: ", Utility::systemErrorMessage()));
    }
    if (!blocking)
    {
        makeSocketNonBlocking(socketId);
    }
}

void BaseSocket::makeSocketNonBlocking(int socketId)
{
    if (sys.fcntl(socketId, F_SETFL, O_NONBLOCK) == -1)
    {
        throw std::domain_error(Utility::buildErrorMessage("ThorsAnvil::Socket::BaseSocket::", __func__,
                                                  ": fcntl: ", Utility::systemErrorMessage()));
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
    if (socketId == invalidSocketId)
    {
        throw std::logic_error(Utility::buildErrorMessage("ThorsAnvil::Socket::BaseSocket::", __func__,
                                                 ": accept called on a bad socket object (this object was moved)"));
    }
    while (true)
    {
        if (sys.close(socketId) == -1)
        {
            switch (errno)
            {
                case EBADF: socketId = invalidSocketId;
                            throw std::domain_error(Utility::buildErrorMessage("ThorsAnvil::Socket::BaseSocket::", __func__,
                                                                      ": close: ", socketId, " ", Utility::systemErrorMessage()));
                case EIO:   socketId = invalidSocketId;
                            throw std::runtime_error(Utility::buildErrorMessage("ThorsAnvil::Socket::BaseSocket::", __func__,
                                                                       ": close: ", socketId, " ", Utility::systemErrorMessage()));
                case EINTR:
                {
                    // TODO: Check for user interrupt flags.
                    //       Beyond the scope of this project
                    //       so continue normal operations.
                    continue;
                }
                default:    socketId = invalidSocketId;
                            throw std::domain_error(Utility::buildErrorMessage("ThorsAnvil::Socket::BaseSocket::", __func__,
                                                                       ": close: ", socketId, " ", Utility::systemErrorMessage()));
            }
        }
        break;
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
    : DataSocket(sys.socket(PF_INET, SOCK_STREAM, 0), true)
{
    SocketAddrIn serverAddr{};

    serverAddr.sin_family       = AF_INET;
    serverAddr.sin_port         = htons(port);
    //serverAddr.sin_addr.s_addr  = inet_addr(host.c_str());

    HostEnt* serv;
    while (true)
    {
        serv = sys.gethostbyname(host.c_str());
        if (serv == nullptr)
        {
            if (h_errno == TRY_AGAIN)
            {
                continue;
            }
            throw std::runtime_error(Utility::buildErrorMessage("ThorsAnvil::Socket::ConnectSocket::", __func__,
                                                       ": gethostbyname: ", Utility::systemErrorMessage()));
        }
        break;
    }
    bcopy((char *)serv->h_addr, (char *)&serverAddr.sin_addr.s_addr, serv->h_length);

    if (sys.connect(getSocketId(), reinterpret_cast<SocketAddr*>(&serverAddr), sizeof(serverAddr)) != 0)
    {
        close();
        throw std::domain_error(Utility::buildErrorMessage("ThorsAnvil::Socket::ConnectSocket::", __func__,
                                                   ": connect: ", Utility::systemErrorMessage()));
    }
}

ServerSocket::ServerSocket(int port, bool blocking)
    : BaseSocket(sys.socket(PF_INET, SOCK_STREAM, 0), blocking)
{
    SocketAddrIn    serverAddr = {};
    serverAddr.sin_family       = AF_INET;
    serverAddr.sin_port         = htons(port);
    serverAddr.sin_addr.s_addr  = INADDR_ANY;

    if (sys.bind(getSocketId(), reinterpret_cast<SocketAddr*>(&serverAddr), sizeof(serverAddr)) != 0)
    {
        close();
        throw std::runtime_error(Utility::buildErrorMessage("ThorsAnvil::Socket::ServerSocket::", __func__,
                                                   ": bind: ", Utility::systemErrorMessage()));
    }

    if (sys.listen(getSocketId(), maxConnectionBacklog) != 0)
    {
        close();
        throw std::runtime_error(Utility::buildErrorMessage("ThorsAnvil::Socket::ServerSocket::", __func__,
                                                   ": listen: ", Utility::systemErrorMessage()));
    }
}

DataSocket ServerSocket::accept(bool blocking)
{
    if (getSocketId() == invalidSocketId)
    {
        throw std::logic_error(Utility::buildErrorMessage("ThorsAnvil::Socket::ServerSocket::", __func__,
                                                 ": accept called on a bad socket object (this object was moved)"));
    }

    int newSocket = sys.accept(getSocketId(), nullptr, nullptr);
    if (newSocket == invalidSocketId)
    {
        throw std::runtime_error(Utility::buildErrorMessage("ThorsAnvil::Socket::ServerSocket:", __func__,
                                                   ": accept: ", Utility::systemErrorMessage()));
    }
    return DataSocket(newSocket, blocking);
}

std::pair<bool, std::size_t> DataSocket::getMessageData(char* buffer, std::size_t size, std::size_t alreadyGot)
{
    if (getSocketId() == invalidSocketId)
    {
        throw std::logic_error(Utility::buildErrorMessage("ThorsAnvil::Socket::DataSocket::", __func__,
                                                 ": getMessageData called on a bad socket object (this object was moved)"));
    }

    std::size_t     dataRead  = alreadyGot;
    while (dataRead < size)
    {
        // The inner loop handles interactions with the socket.
        std::size_t get = sys.read(getSocketId(), buffer + dataRead, size - dataRead);
        if (get == static_cast<std::size_t>(-1))
        {
            switch (errno)
            {
                case EBADF:
                case EFAULT:
                case EINVAL:
                case ENXIO:
                case ENOMEM:
                {
                    // Fatal error. Programming bug
                    throw std::domain_error(Utility::buildErrorMessage("ThorsAnvil::Socket::DataSocket::", __func__,
                                                              ": read: critical error: ", Utility::systemErrorMessage()));
                }
                case EIO:
                case ENOBUFS:
                {
                   // Resource acquisition failure or device error
                    throw std::runtime_error(Utility::buildErrorMessage("ThorsAnvil::Socket::DataSocket::", __func__,
                                                               ": read: resource failure: ", Utility::systemErrorMessage()));
                }
                case EINTR:
                {
                    // TODO: Check for user interrupt flags.
                    //       Beyond the scope of this project
                    //       so continue normal operations.
                    continue;
                }
                case ETIMEDOUT:
                case EAGAIN:
                //case EWOULDBLOCK:
                {
                    // Temporary error.
                    // Simply retry the read.
                    return {true, dataRead - alreadyGot};
                }
                case ECONNRESET:
                case ENOTCONN:
                {
                    // Connection broken.
                    // Return the data we have available and exit
                    // as if the connection was closed correctly.
                    return {false, dataRead - alreadyGot};
                }
                default:
                {
                    throw std::runtime_error(Utility::buildErrorMessage("ThorsAnvil::Socket::DataSocket::", __func__,
                                                               ": read: returned -1: ", Utility::systemErrorMessage()));
                }
            }
        }
        if (get == 0)
        {
            return {false, dataRead - alreadyGot};
        }
        dataRead += get;
    }

    return {true, dataRead - alreadyGot};
}

std::pair<bool, std::size_t> DataSocket::putMessageData(char const* buffer, std::size_t size, std::size_t alreadyPut)
{
    if (getSocketId() == invalidSocketId)
    {
        throw std::logic_error(Utility::buildErrorMessage("ThorsAnvil::Socket::DataSocket::", __func__,
                                                 ": putMessageData called on a bad socket object (this object was moved)"));
    }

    std::size_t     dataWritten = alreadyPut;

    while (dataWritten < size)
    {
        std::size_t put = sys.write(getSocketId(), buffer + dataWritten, size - dataWritten);
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
                    throw std::domain_error(Utility::buildErrorMessage("ThorsAnvil::Socket::DataSocket::", __func__,
                                                              ": write: critical error: ", Utility::systemErrorMessage()));
                }
                case EDQUOT:
                case EFBIG:
                case EIO:
                case ENETDOWN:
                case ENETUNREACH:
                case ENOSPC:
                {
                    // Resource acquisition failure or device error
                    throw std::runtime_error(Utility::buildErrorMessage("ThorsAnvil::Socket::DataSocket::", __func__,
                                                               ": write: resource failure: ", Utility::systemErrorMessage()));
                }
                case EINTR:
                {
                    // TODO: Check for user interrupt flags.
                    //       Beyond the scope of this project
                    //       so continue normal operations.
                    continue;
                }
                case ETIMEDOUT:
                case EAGAIN:
                //case EWOULDBLOCK:
                {
                    // Temporary error.
                    // Simply retry the read.
                    return {true, dataWritten - alreadyPut};
                }
                default:
                {
                    throw std::runtime_error(Utility::buildErrorMessage("ThorsAnvil::Socket::DataSocket::", __func__,
                                                               ": write: returned -1: ", Utility::systemErrorMessage()));
                }
            }
        }
        dataWritten += put;
    }
    return {true, dataWritten - alreadyPut};
}

void DataSocket::putMessageClose()
{
    if (sys.shutdown(getSocketId(), SHUT_WR) != 0)
    {
        throw std::domain_error(Utility::buildErrorMessage("ThorsAnvil::Socket::DataSocket::", __func__,
                                                  ": shutdown: critical error: ", Utility::systemErrorMessage()));
    }
}
