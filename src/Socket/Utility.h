#ifndef THORSANVIL_SOCKET_UTILITY_H
#define THORSANVIL_SOCKET_UTILITY_H

#include <string>
#include <sstream>
#include <utility>
#include <cstddef>
#include <cerrno>

namespace ThorsAnvil
{
    namespace Socket
    {

template<typename... Args>
void print(std::ostream& s, Args&... args)
{
    using Expander = int[];
    Expander ignore{ 0, (s << args, 0)...};
    (void)ignore; // Cast to ignore to avoid compiler warnings.
}

template<typename... Args>
std::string buildStringFromParts(Args const&... args)
{
    std::stringstream msg;
    print(msg, args...);
    return msg.str();
}

template<typename... Args>
std::string buildErrorMessage(Args const&... args)
{
    return buildStringFromParts(args...);
}

inline std::string errnoToName()
{
    switch (errno)
    {
#define  THOR_CASE(x)  case x: return #x ""
        THOR_CASE( EACCES );
        THOR_CASE( EADDRINUSE );
        THOR_CASE( EADDRNOTAVAIL );
        THOR_CASE( EAFNOSUPPORT );
        THOR_CASE( EAGAIN );
        // TODO );
        //THOR_CASE( EWOULDBLOCK );
        THOR_CASE( EBADF );
        THOR_CASE( ECONNABORTED );
        THOR_CASE( ECONNRESET );
        THOR_CASE( EDESTADDRREQ );
        THOR_CASE( EDQUOT );
        THOR_CASE( EEXIST );
        THOR_CASE( EFAULT );
        THOR_CASE( EFBIG );
        THOR_CASE( EINTR );
        THOR_CASE( EINVAL );
        THOR_CASE( EIO );
        THOR_CASE( EISDIR );
        THOR_CASE( ELOOP );
        THOR_CASE( EMFILE );
        THOR_CASE( ENAMETOOLONG );
        THOR_CASE( ENETDOWN );
        THOR_CASE( ENETUNREACH );
        THOR_CASE( ENFILE );
        THOR_CASE( ENOBUFS );
        THOR_CASE( ENOENT );
        THOR_CASE( ENOMEM );
        THOR_CASE( ENOSPC );
        THOR_CASE( ENOTCONN );
        THOR_CASE( ENOTDIR );
        THOR_CASE( ENOTSOCK );
        THOR_CASE( ENXIO );
        THOR_CASE( EOPNOTSUPP );
        THOR_CASE( EPIPE );
        THOR_CASE( EPROTONOSUPPORT );
        THOR_CASE( EPROTOTYPE );
        THOR_CASE( EROFS );
        THOR_CASE( ESPIPE );
        THOR_CASE( ETIMEDOUT );
        THOR_CASE( EALREADY );
        THOR_CASE( ECONNREFUSED );
        THOR_CASE( EHOSTUNREACH );
        THOR_CASE( EINPROGRESS );
        THOR_CASE( EISCONN );
#undef THOR_CASE
    }
    return "Unknown: ";
}

inline std::string systemErrorMessage()
{
    std::string result = errnoToName();
    result += strerror(errno);
    return result;
}

    }
}

#endif
