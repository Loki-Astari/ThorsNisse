#ifndef THORSANVIL_NISSE_CORE_SERVICE_SERVER_H
#define THORSANVIL_NISSE_CORE_SERVICE_SERVER_H

/** LibDesc:
A simple wrapper around libEvent.
@ example example/LibDesc.cpp
*/

#include "EventUtil.h"
#include "ThorsNisseCoreSocket/Socket.h"
#include <memory>
#include <unordered_map>
#include <functional>
#include <string>
#include <vector>

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

// @class
// Used to simplify the definition of a port.
struct ServerConnection
{
    int port;
    int maxConnections;
    public:
        // @method
        ServerConnection(int port, int maxConnections = ThorsAnvil::Nisse::Core::Socket::ServerSocket::maxConnectionBacklog)
            : port(port)
            , maxConnections(maxConnections)
        {}
};

// @class
// An object that acts as the main server event loop.
// One of these objects can handle all the ports your application requires
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
        // @method
        Server();

        // @method
        Server(Server&&);
        // @method
        Server& operator=(Server&&);

        // @method
        // Starts the event loop.
        // This method does not return immediately. A call to <code>flagShutDown()</code> will cause the event loop to exit after the current iteration.
        // @param Timeout period after which internal house keeping operations are performed.
        void start(double check = 10.0);
        // @method
        // Marks the event loop for shut down.
        // After the current iteration of the event loop has finished it will exit. This will cause the `start()` function to return.
        void flagShutDown();

        template<typename Handler>
        void listenOn(ServerConnection const& info);
        // @method
        // This is a templatized function. The template type is the type of object that will handle a connection once it has been established.
        // When a connection is accepted and object of the template type is created and passed a reference to the server, stream and `data` (see parameters).
        // The object is marked as listening to the accepted socket and when data is available on the socket the <code>eventActivate()</code> method is called allowing the data to be processed.
        // For more details see <a href="#HandlerBase">HandlerBase</a>.
        // @param info  The port that is being listen too.
        // @param param A reference to an object that is passed to the constructor of the handler type. This allows a state object to be passed to the constructor.
        template<typename Handler, typename Param>
        void listenOn(ServerConnection const& info, Param& param);

        // @method
        // Sets a timer to go off every `timeOut` seconds.
        // The result of the timmer going off is to execute the functot `action`.
        // @param timeOut The time period (in seconds)  between running the action object.
        // @param action  Functor that is run every `timeOut` seconds.
        void addTimer(double timeOut, std::function<void()>&& action);

        bool isRunning() const {return running;}
        template<typename H, typename... Args>
        void transferHandler(Args&&... args);

        static Server&       getCurrentHandler();
        static bool          inHandler();
        static void ignore(std::string const& type = "");
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
