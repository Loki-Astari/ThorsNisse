#ifndef THORSANVIL_NISSE_CORE_SOCKET_SOCKET_H
#define THORSANVIL_NISSE_CORE_SOCKET_SOCKET_H

#include <string>
#include <utility>
#include <functional>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>


using SocketAddr    = struct sockaddr;
using SocketStorage = struct sockaddr_storage;
using SocketAddrIn  = struct sockaddr_in;
using HostEnt       = struct hostent;

inline int closeWrapper(int fd)                             {return ::close(fd);}
inline int socketWrapper(int family, int type, int protocol){return ::socket(family, type, protocol);}
inline int acceptWrapper(int sockfd, sockaddr* addr, socklen_t* len) {return ::accept(sockfd, addr, len);}
inline ssize_t readWrapper(int fd, void* buf, size_t count) {return ::read(fd, buf, count);}
inline ssize_t writeWrapper(int fd, void const* buf, size_t count){return ::write(fd, buf, count);}
inline int fcntlWrapper(int fd, int cmd, int value)         {return ::fcntl(fd, cmd, value);}

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace Core
        {
            namespace Socket
            {

// @class
// Base of all the socket classes.
// This class should not be directly created.
// All socket classes are movable but not copyable.
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
        // @method
        BaseSocket(BaseSocket&& move)               noexcept;
        // @method
        BaseSocket& operator=(BaseSocket&& move)    noexcept;
        // @method
        void swap(BaseSocket& other)                noexcept;
        BaseSocket(BaseSocket const&)               = delete;
        BaseSocket& operator=(BaseSocket const&)    = delete;

        // User can manually call close
        // @method
        void close();
    private:
        static void makeSocketNonBlocking(int socketId);
};

// @class
// Data sockets define the read/write interface to a socket.
// This class should not be directly created
class DataSocket: public BaseSocket
{
    public:
        // @method
        DataSocket(int socketId, bool blocking = false)
            : BaseSocket(socketId, blocking)
        {}

        // @method
        // Reads data from a sokcet into a buffer.
        // If the stream is blocking will not return until the requested amount of data has been read or there is no more data to read.
        // If the stream in non blocking will return if the read operation would block.
        // @return              This method returns a <code>std::pair&lt;bool, std::size_t&gt;</code>. The first member `bool` indicates if more data can potentially be read from the stream. If the socket was cut or the EOF reached then this value will be false. The second member `std::size_t` indicates exactly how many bytes were read from this stream.
        // @param buffer        The buffer data will be read into.
        // @param size          The size of the buffer.
        // @param alreadyGot    Offset into buffer (and amount size is reduced by) as this amount was read on a previous call).
        std::pair<bool, std::size_t> getMessageData(char* buffer, std::size_t size, std::size_t alreadyGot = 0);
        // @method
        // Writes data from a buffer to a sokcet.
        // If the stream is blocking will not return until the requested amount of data has been written or the socket was closed to writting.
        // If the stream in non blocking will return if the write operation would block.
        // @return              This method returns a <code>std::pair&lt;bool, std::size_t&gt;</code>. The first member `bool` indicates if more data can potentially be written to the stream. If the socket was cut or closed then this value will be false. The second member `std::size_t` indicates exactly how many bytes were written to this stream.
        // @param buffer        The buffer data will be written from.
        // @param size          The size of the buffer.
        // @param alreadyPut    Offset into buffer (and amount size is reduced by) as this amount was written on a previous call).
        std::pair<bool, std::size_t> putMessageData(char const* buffer, std::size_t size, std::size_t alreadyPut = 0);
        // @method
        // closes the write end of the socket and flushes (write) data.
        // @return              closes the write end of the socket and flushes (write) data.
        void        putMessageClose();
};

// @class
// Creates a connection to <code>host</code> on <code>port</code>.
// Note this class inherits from <code>DataSocket</code> so once created you can read/write to the socket.
class ConnectSocket: public DataSocket
{
    public:
        // @method
        ConnectSocket(std::string const& host, int port);
};

// @class
// A server socket that listens on a port for a connection
class ServerSocket: public BaseSocket
{
    public:
        static constexpr int maxConnectionBacklog = 5;
        // @method
        ServerSocket(int port, bool blocking = false, int maxWaitingConnections = maxConnectionBacklog);

        // @method
        // "Accept" a waiting connection request on the port and creates a two way socket for communication on another port.
        // If this is a non blocking socket and there is no waiting connection an exception is thrown.
        // If this is a blocking socket wait for a connection.
        // @return              A <code>DataSocket</code> is returned so data can be exchange across the socket.
        // @param blocking      Passed to the constructor of the <code>DataSocket</code> that is returned.
        DataSocket accept(bool blocking = false);
};

            }
        }
    }
}

#endif
