#ifndef THORSANVIL_NISSE_CORE_SERVICE_CONTEXT_H
#define THORSANVIL_NISSE_CORE_SERVICE_CONTEXT_H

#include "NisseConfig.h"

#if BOOST_COROUTINE_VERSION == 1 || BOOST_COROUTINE_VERSION == 2
#include <boost/coroutine/all.hpp>
#elif BOOST_COROUTINE_VERSION == 3
#include <boost/coroutine2/all.hpp>
#else
#error "Unknown Co-Routine Version"
#endif

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace Core
        {
            namespace Service
            {

#if BOOST_COROUTINE_VERSION == 1
template<typename R>
struct Context
{
    using pull_type = typename boost::coroutines::coroutine<R()>;
    using push_type = typename pull_type::caller_type;
};

#elif BOOST_COROUTINE_VERSION == 2
template<typename R>
using Context = typename boost::coroutines::asymmetric_coroutine<R>;

#elif BOOST_COROUTINE_VERSION == 3
template<typename R>
using Context = typename boost::coroutines2::coroutine<R>;

#endif

            }
        }
    }
}

#endif
