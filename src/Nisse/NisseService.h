#ifndef THORSANVIL_NISSE_NISSE_SERVICE_H
#define THORSANVIL_NISSE_NISSE_SERVICE_H

#include "NisseEventUtil.h"
#include "ThorsNisseSocket/Socket.h"
#include <memory>
#include <unordered_map>
#include <functional>
#include <string>

namespace ThorsAnvil
{
    namespace Nisse
    {

class NisseService;
class NisseHandler;
using EventBaseDeleter  = decltype(&event_base_free);
using EventHolder       = std::unique_ptr<LibEventBase, EventBaseDeleter>;
using NisseManagHandler = std::unique_ptr<NisseHandler>;

class NisseService
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
        std::unordered_map<NisseHandler*, NisseManagHandler>  handlers;

        // This vector is a non owning vector.
        // It contains the pointers to handlers that need to be
        // deleted. We don't want handlers to delete themselves
        // so they register themselves for deletion and get purged
        // in `runLoop()`.
        std::vector<NisseHandler*>      retiredHandlers;
        NisseHandler*                   currentHandler;
        static NisseService*            currentService;
    public:
        NisseService();

        NisseService(NisseService&&);
        NisseService& operator=(NisseService&&);

        void start(double check = 10.0);
        void flagShutDown();

        template<typename Handler>
        void listenOn(int port);
        template<typename Handler, typename Param>
        void listenOn(int port, Param& param);

        void addTimer(double timeOut, std::function<void()>&& action);

        using EventConfig = struct event_config;
        static EventConfig*  cfg;
        static void ignore(std::string const& type = "")
        {
            if (cfg != nullptr)
            {
                event_config_free(cfg);
                cfg = nullptr;
            }
            if (type != "")
            {
                cfg = event_config_new();
                event_config_avoid_method(cfg, type.c_str());
            }
        }
        bool isRunning() const {return running;}
        template<typename H, typename... Args>
        void transferHandler(Args&&... args);

        static NisseService& getCurrentHandler() {return *currentService;}
    private:
        void runLoop(double check);
        void purgeRetiredHandlers();
        void swap(NisseService& );

        friend class NisseHandler;
        template<typename H, typename... Args>
        NisseHandler& addHandler(Args&&... args);
        void delHandler(NisseHandler* oldHandler);
        void setCurrentHandler(NisseHandler* current);
};

    }
}

#ifndef COVERAGE_TEST
#include "NisseService.tpp"
#endif
#endif
