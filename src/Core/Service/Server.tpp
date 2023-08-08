#ifndef THORSANVIL_NISSE_NISSE_SERVICE_SERVER_TPP
#define THORSANVIL_NISSE_NISSE_SERVICE_SERVER_TPP

#ifndef THORSANVIL_NISSE_CORE_SERVICE_SERVICE_H
#error "Please do not include >ThorsNisseCoreService/Server.tpp< directly. Include the >ThorsNisseCoreService/Service.h< file instead."
#endif

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace Core
        {
            namespace Service
            {

template<typename Handler, typename Param> class ServerHandler;
template<typename Handler>
inline void Server::listenOn(ServerConnection const& info)
{
    addHandler<ServerHandler<Handler, void>>(Socket::ServerSocket(info.port, false, info.maxConnections));
}

template<typename Handler, typename Param>
inline void Server::listenOn(ServerConnection const& info, Param& param)
{
    /** MethodDesc:
    This is a templatized function. The template type is the type of object that will handle a connection once it has been established.
    When a connection is accepted and object of the template type is created and passed a reference to the server, stream and `data` (see parameters)
    The object is marked as listening to the accepted socket and when data is available on the socket the <code>eventActivate()</code> method is called allowing the data to be processed.
    For more details see <a href=\"#HandlerBase\">HandlerBase</a>.
    @ param port The port that is being listen too.
    @ param param An optional parameter: A reference to an object that is passed to the constructor of the handler type. This allows a state object to be passed to the constructor.
    @ example example/Server-listenOn.cpp
    */
    addHandler<ServerHandler<Handler, Param>>(Socket::ServerSocket(info.port, false, info.maxConnections), param);
}

template<typename H, typename... Args>
inline HandlerBase& Server::addHandler(Args&&... args)
{
    NisseManagHandler   value = std::make_unique<H>(*this, std::forward<Args>(args)...);
    HandlerBase*        key   = value.get();
    handlers.emplace(key, std::move(value));
    return *key;
}

template<typename H, typename... Args>
inline void Server::transferHandler(Args&&... args)
{
    if (currentHandler == nullptr || !currentHandler->suspendable())
    {
        throw std::runtime_error("Can not transfer handlers when not running a current handler (or the current one is non-suspendable)");
    }
    HandlerBase& handler = addHandler<H>(std::forward<Args>(args)...);
    handler.setSuspend(*currentHandler);
}

            }
        }
    }
}

#endif
