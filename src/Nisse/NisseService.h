#ifndef THORSANVIL_NISSE_NISSE_SERVICE_H
#define THORSANVIL_NISSE_NISSE_SERVICE_H

#include "NisseEventUtil.h"

namespace ThorsAnvil
{
    namespace Nisse
    {

class NisseService
{
    private:

        bool            running;
        LibEventBase*   eventBase;
    public:
        ~NisseService();
        NisseService();

        NisseService(NisseService const&)               = delete;
        NisseService(NisseService&&)                    = delete;
        NisseService& operator=(NisseService const&)    = delete;
        NisseService& operator=(NisseService&&)         = delete;

        void start();
        void flagShutDown();

        void listenOn(int port);
    private:
        void runLoop();
};

    }
}

#endif
