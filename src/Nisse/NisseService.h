#ifndef THORSANVIL_NISSE_NISSE_SERVICE_H
#define THORSANVIL_NISSE_NISSE_SERVICE_H

#include <event.h>

namespace ThorsAnvil
{
    namespace Nisse
    {

using EventBase = struct event_base;
class NisseService
{
    private:

        bool            running;
        EventBase*      eventBase;
    public:
        ~NisseService();
        NisseService();

        NisseService(NisseService const&)               = delete;
        NisseService(NisseService&&)                    = delete;
        NisseService& operator=(NisseService const&)    = delete;
        NisseService& operator=(NisseService&&)         = delete;

        void start();
        void flagShutDown();
    private:
        void runLoop();
};

    }
}

#endif
