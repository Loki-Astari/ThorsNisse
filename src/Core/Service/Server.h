#ifndef THORSANVIL_NISSE_CORE_SERVICE_SERVER_H
#define THORSANVIL_NISSE_CORE_SERVICE_SERVER_H

#include "EventUtil.h"
#include "ThorsNisseCoreSocket/Socket.h"
#include <memory>
#include <unordered_map>
#include <functional>
#include <string>
#include <vector>
#include <event.h>

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace Core
        {
            namespace Service
            {

class Server;
class HandlerBase;
using EventBaseDeleter  = std::function<void(LibEventBase*)>;

using EventHolder       = std::unique_ptr<LibEventBase, EventBaseDeleter>;
using NisseManagHandler = std::unique_ptr<HandlerBase>;

struct ServerConnection
{
    int port;
    int maxConnections;
    public:
        ServerConnection(int port, int maxConnections = ThorsAnvil::Nisse::Core::Socket::ServerSocket::maxConnectionBacklog)
            : port(port)
            , maxConnections(maxConnections)
        {}
};

class Server
{
    private:
        bool                            running;
        bool                            shutDownNext;
        EventHolder                     eventBase;
        // The handlers are held by pointer because they are
        // polymorphic (there can potentially be a lot of different
        // handlers for different incoming streams).
        // We use an unordered map to make it easy to find the handler
        // when we want to delete it.
        std::unordered_map<HandlerBase*, NisseManagHandler>  handlers;

        // This vector is a non owning vector.
        // It contains the pointers to handlers that need to be
        // deleted. We don't want handlers to delete themselves
        // so they register themselves for deletion and get purged
        // in `runLoop()`.
        std::vector<HandlerBase*>       retiredHandlers;
        HandlerBase*                    currentHandler;
        static Server*                  currentService;
        static LibEventConfig*          cfg;
    public:
        Server();

        Server(Server&&);
        Server& operator=(Server&&);

        void start(double check = 10.0);
        void flagShutDown();

        template<typename Handler>
        void listenOn(ServerConnection const& info);
        template<typename Handler, typename Param>
        void listenOn(ServerConnection const& info, Param& param);

        void addTimer(double timeOut, std::function<void()>&& action);

        bool isRunning() const {return running;}
        template<typename H, typename... Args>
        void transferHandler(Args&&... args);

        static Server&       getCurrentHandler();
        static bool          inHandler();
        static void ignore(std::string const& type = "")
        {
            if (cfg != nullptr)
            {
                ::event_config_free(cfg);
                cfg = nullptr;
            }
            if (type != "")
            {
                cfg = ::event_config_new();
                ::event_config_avoid_method(cfg, type.c_str());
            }
        }
    private:
        void runLoop(double check);
        void purgeRetiredHandlers();
        void swap(Server& other);

        friend class HandlerBase;
        template<typename H, typename... Args>
        HandlerBase& addHandler(Args&&... args);
        void delHandler(HandlerBase* oldHandler);
        void setCurrentHandler(HandlerBase* current);
};

            }
        }
    }
}

#ifndef COVERAGE_TEST
#include "Server.tpp"
#endif
#endif
