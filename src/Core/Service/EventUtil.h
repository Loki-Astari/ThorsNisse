#ifndef THORSANVIL_NISSE_CORE_SERVICE_NISSE_EVENT_UTIL_H
#define THORSANVIL_NISSE_CORE_SERVICE_NISSE_EVENT_UTIL_H

#include <event.h>

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace Core
        {
            namespace Service
            {

using LibEventBase  = struct event_base;
using LibEvent      = struct event;
using LibSocketId   = evutil_socket_t;
using TimeVal       = struct timeval;

            }
        }
    }
}

#endif
