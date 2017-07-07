#include "NisseService.h"
#include "NisseHandler.h"

#include <iostream>

using namespace ThorsAnvil::Nisse;

NisseService::NisseService()
    : running(false)
    , eventBase(event_base_new(), &event_base_free)
{
    if (eventBase == nullptr)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::NisseService::NisseService: event_base_new(): Failed");
    }
}

NisseService::NisseService(NisseService&& move) noexcept
    : eventBase(nullptr, &event_base_free)
{
    swap(move);
}

NisseService& NisseService::operator=(NisseService&& move) noexcept
{
    swap(move);
    return *this;
}

void NisseService::swap(NisseService& other) noexcept
{
    using std::swap;
    swap(running,           other.running);
    swap(eventBase,         other.eventBase);
    swap(handlers,          other.handlers);
    swap(retiredHandlers,   other.retiredHandlers);
}

void NisseService::start()
{
    std::cout << "Nisse Started\n";
    running = true;
    while (running)
    {
        std::cout << "Nisse Loop\n";
        runLoop();
    }
    std::cout << "Nisse Stopped\n";
}

void NisseService::flagShutDown()
{
    running = false;
    if (event_base_loopbreak(eventBase.get()) != 0)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::NisseService::flagShutDown: event_base_loopbreak(): Failed");
    }
}

void NisseService::runLoop()
{
    static const TimeVal ten_sec{10,0};

    if (event_base_loopexit(eventBase.get(), &ten_sec) != 0)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::NisseService::runLoop: event_base_loopexit(): Failed");
    }
    switch (event_base_dispatch(eventBase.get()))
    {
        case -1:
            throw std::runtime_error("ThorsAnvil::Nisse::NisseService::runLoop: event_base_dispatch(): Failed");
        case 1:
            throw std::runtime_error("ThorsAnvil::Nisse::NisseService::runLoop: event_base_dispatch(): No Events");
        case 0:
            break;
        default:
            throw std::runtime_error("ThorsAnvil::Nisse::NisseService::runLoop: event_base_dispatch(): Unknown Error");
    }
}

void NisseService::delHandler(NisseHandler* oldHandler)
{
    retiredHandlers.emplace_back(oldHandler);
}

#ifdef COVERAGE_TEST
/*
 * This code is only compiled into the unit tests for code coverage purposes
 * It is not part of the live code.
 */
#include "NisseService.tpp"
#endif
