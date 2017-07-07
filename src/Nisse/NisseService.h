#ifndef THORSANVIL_NISSE_NISSE_SERVICE_H
#define THORSANVIL_NISSE_NISSE_SERVICE_H

#include "NisseEventUtil.h"
#include "ThorsSocket/Socket.h"
#include <memory>
#include <vector>

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
        std::vector<NisseManagHandler>  handlers;
        std::vector<NisseHandler*>      retiredHandlers;
    public:
        NisseService();

        NisseService(NisseService&&) noexcept;
        NisseService& operator=(NisseService&&) noexcept;

        void start();
        void flagShutDown();

        template<typename Handler>
        void listenOn(int port);
    private:
        void runLoop();
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
