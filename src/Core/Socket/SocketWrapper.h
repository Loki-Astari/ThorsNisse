#ifndef THORSANVIL_NISSE_CORE_SOCKET_SOCKET_WRAPPER_H
#define THORSANVIL_NISSE_CORE_SOCKET_SOCKET_WRAPPER_H

inline int closeWrapper(int fd)                             {return ::close(fd);}
inline int socketWrapper(int family, int type, int protocol){return ::socket(family, type, protocol);}
inline int acceptWrapper(int sockfd, sockaddr* addr, socklen_t* len) {return ::accept(sockfd, addr, len);}
inline ssize_t readWrapper(int fd, void* buf, size_t count) {return ::read(fd, buf, count);}
inline ssize_t writeWrapper(int fd, void const* buf, size_t count){return ::write(fd, buf, count);}
inline int fcntlWrapper(int fd, int cmd, int value)         {return ::fcntl(fd, cmd, value);}
inline int shutdownWrapper(int fd, int how)                 {return ::shutdown(fd, how);}


#endif
