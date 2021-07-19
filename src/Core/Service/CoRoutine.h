#ifndef THORSANVIL_NISSE_CORE_SERVICE_CONTEXT_H
#define THORSANVIL_NISSE_CORE_SERVICE_CONTEXT_H

#include "CoreServiceConfig.h"
#include <boost/coroutine/all.hpp>

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace Core
        {
            namespace Service
            {

template<typename R>
using Context = typename boost::coroutines::asymmetric_coroutine<R>;

            }
        }
    }
}

#endif
