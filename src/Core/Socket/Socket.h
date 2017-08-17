#ifndef THORSANVIL_NISSE_CORE_SOCKET_SOCKET_H
#define THORSANVIL_NISSE_CORE_SOCKET_SOCKET_H

#include <string>
#include <utility>

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

            }
        }
    }
}

#endif