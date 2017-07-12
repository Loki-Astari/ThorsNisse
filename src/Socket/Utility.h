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
        case EACCES:            return "EACCES ";
        case EADDRINUSE:        return "EADDRINUSE ";
        case EADDRNOTAVAIL:     return "EADDRNOTAVAIL ";
        case EAFNOSUPPORT:      return "EAFNOSUPPORT ";
        case EAGAIN:            return "EAGAIN ";
        // TODO: Add configuration test
        //case EWOULDBLOCK:       return "EWOULDBLOCK ";
        case EBADF:             return "EBADF ";
        case ECONNABORTED:      return "ECONNABORTED ";
        case ECONNRESET:        return "ECONNRESET ";
        case EDESTADDRREQ:      return "EDESTADDRREQ ";
        case EDQUOT:            return "EDQUOT ";
        case EEXIST:            return "EEXIST ";
        case EFAULT:            return "EFAULT ";
        case EFBIG:             return "EFBIG ";
        case EINTR:             return "EINTR ";
        case EINVAL:            return "EINVAL ";
        case EIO:               return "EIO ";
        case EISDIR:            return "EISDIR ";
        case ELOOP:             return "ELOOP ";
        case EMFILE:            return "EMFILE ";
        case ENAMETOOLONG:      return "ENAMETOOLONG ";
        case ENETDOWN:          return "ENETDOWN ";
        case ENETUNREACH:       return "ENETUNREACH ";
        case ENFILE:            return "ENFILE ";
        case ENOBUFS:           return "ENOBUFS ";
        case ENOENT:            return "ENOENT ";
        case ENOMEM:            return "ENOMEM ";
        case ENOSPC:            return "ENOSPC ";
        case ENOTCONN:          return "ENOTCONN ";
        case ENOTDIR:           return "ENOTDIR ";
        case ENOTSOCK:          return "ENOTSOCK ";
        case ENXIO:             return "ENXIO ";
        case EOPNOTSUPP:        return "EOPNOTSUPP ";
        case EPIPE:             return "EPIPE ";
        case EPROTONOSUPPORT:   return "EPROTONOSUPPORT ";
        case EPROTOTYPE:        return "EPROTOTYPE ";
        case EROFS:             return "EROFS ";
        case ESPIPE:            return "ESPIPE ";
        case ETIMEDOUT:         return "ETIMEDOUT ";
        case EALREADY:          return "EALREADY ";
        case ECONNREFUSED:      return "ECONNREFUSED ";
        case EHOSTUNREACH:      return "EHOSTUNREACH ";
        case EINPROGRESS:       return "EINPROGRESS ";
        case EISCONN:           return "EISCONN ";
        default:
            break;
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
