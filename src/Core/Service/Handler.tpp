#ifndef THORSANVIL_NISSE_NISSE_HANDLER_TPP
#define THORSANVIL_NISSE_NISSE_HANDLER_TPP

#include "Server.h"

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace Core
        {
            namespace Service
            {

template<typename H, typename... Args>
inline void HandlerBase::addHandler(Args&&... args)
{
    parent.addHandler<H>(std::forward<Args>(args)...);
}

template<typename H, typename... Args>
inline void HandlerBase::moveHandler(Args&&... args)
{
    doDropHandler(false);
    parent.addHandler<H>(std::forward<Args>(args)...);
}

            }
        }
    }
}

#endif
