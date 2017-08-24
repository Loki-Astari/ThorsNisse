#ifndef THORSANVIL_NISSE_CORE_SOCKET_SOCKET_H
#define THORSANVIL_NISSE_CORE_SOCKET_SOCKET_H

#include <string>
#include <utility>
#include <functional>
#include <sys/socket.h>

using SocketAddr    = struct sockaddr;
using SocketStorage = struct sockaddr_storage;
using SocketAddrIn  = struct sockaddr_in;
using HostEnt       = struct hostent;

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace Core
        {
            namespace Socket
            {

// An RAII base class for handling sockets.
// Socket is movable but not copyable.
class BaseSocket
{

    int     socketId;
    protected:
        static constexpr int invalidSocketId      = -1;

        BaseSocket();
        // Designed to be a base class not used used directly.
        BaseSocket(int socketId, bool blocking = false);
    public:
        int getSocketId() const {return socketId;}
    public:
        virtual ~BaseSocket();

        // Moveable but not Copyable
        BaseSocket(BaseSocket&& move)               noexcept;
        BaseSocket& operator=(BaseSocket&& move)    noexcept;
        void swap(BaseSocket& other)                noexcept;
        BaseSocket(BaseSocket const&)               = delete;
        BaseSocket& operator=(BaseSocket const&)    = delete;

        // User can manually call close
        void close();
    private:
        static void makeSocketNonBlocking(int socketId);
};

// A class that can read/write to a socket
class DataSocket: public BaseSocket
{
    public:
        DataSocket(int socketId, bool blocking = false)
            : BaseSocket(socketId, blocking)
        {}

        std::pair<bool, std::size_t> getMessageData(char* buffer, std::size_t size, std::size_t alreadyGot = 0);
        std::pair<bool, std::size_t> putMessageData(char const* buffer, std::size_t size, std::size_t alreadyPut = 0);
        void        putMessageClose();
};

// A class the conects to a remote machine
// Allows read/write accesses to the remote machine
class ConnectSocket: public DataSocket
{
    public:
        ConnectSocket(std::string const& host, int port);
};

// A server socket that listens on a port for a connection
class ServerSocket: public BaseSocket
{
    static constexpr int maxConnectionBacklog = 5;
    public:
        ServerSocket(int port, bool blocking = false);

        // An accepts waits for a connection and returns a socket
        // object that can be used by the client for communication
        DataSocket accept(bool blocking = false);
};

namespace Detail
{

struct SocketInterface
{
    std::function<int(int, int, int)>                         fcntl;
    std::function<int(int)>                                   close;
    std::function<int(int, int, int)>                         socket;
    std::function<int(int, SocketAddr*, std::size_t)>         connect;
    std::function<int(int, SocketAddr*, std::size_t)>         bind;
    std::function<int(int, int)>                              listen;
    std::function<int(int, SocketAddr*, socklen_t*)>          accept;
    std::function<std::size_t(int, void*, std::size_t)>       read;
    std::function<std::size_t(int, void const*, std::size_t)> write;
    std::function<int(int, int)>                              shutdown;
    std::function<HostEnt*(char const* host)>                 gethostbyname;
};

}

            }
        }
    }
}

#endif
