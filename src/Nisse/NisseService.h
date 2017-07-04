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

class NisseHandler;
class NisseService
{
    private:
        using NisseManagHandler = std::unique_ptr<NisseHandler>;
        bool                            running;
        LibEventBase*                   eventBase;
        std::vector<NisseManagHandler>  handlers;
        std::vector<NisseHandler*>      retiredHandlers;
    public:
        ~NisseService();
        NisseService();

        NisseService(NisseService const&)               = delete;
        NisseService(NisseService&&)                    = delete;
        NisseService& operator=(NisseService const&)    = delete;
        NisseService& operator=(NisseService&&)         = delete;

        void start();
        void flagShutDown();

        template<typename Handler>
        void listenOn(int port);
    private:
        void runLoop();

    public:
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
