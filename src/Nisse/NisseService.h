#ifndef THORSANVIL_NISSE_NISSE_SERVICE_H
#define THORSANVIL_NISSE_NISSE_SERVICE_H

#include "NisseEventUtil.h"
#include "ThorsSocket/Socket.h"
#include <memory>
#include <unordered_map>

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
    public:
        NisseService();

        NisseService(NisseService&&) noexcept;
        NisseService& operator=(NisseService&&) noexcept;

        void start();
        void flagShutDown();

        template<typename Handler>
        void listenOn(int port);
        template<typename Handler, typename Param>
        void listenOn(int port, Param& param);
    private:
        void runLoop();
        void purgeRetiredHandlers();
        void swap(NisseService& ) noexcept;

        friend class NisseHandler;
        template<typename H, typename... Args>
        void addHandler(Args&&... args);
        void delHandler(NisseHandler* oldHandler);
};

    }
}

#ifndef COVERAGE_TEST
#include "NisseService.tpp"
#endif
#endif
