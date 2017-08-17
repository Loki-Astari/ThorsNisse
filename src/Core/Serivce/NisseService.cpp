#include "NisseService.h"
#include "NisseHandler.h"

#include <iostream>

using namespace ThorsAnvil::Nisse::Core::Service;

EventConfig*  NisseService::cfg              = nullptr;
NisseService* NisseService::currentService   = nullptr;

NisseService::NisseService()
    : running(false)
    , shutDownNext(false)
    , eventBase(event_base_new_with_config(cfg), &event_base_free)
    , currentHandler(nullptr)
{
    if (eventBase == nullptr)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::NisseService::NisseService: event_base_new(): Failed");
    }
}

NisseService::NisseService(NisseService&& move)
    : running(false)
    , shutDownNext(false)
    , eventBase(nullptr, &event_base_free)
    , currentHandler(nullptr)
{
    swap(move);
}

NisseService& NisseService::operator=(NisseService&& move)
{
    swap(move);
    return *this;
}

void NisseService::swap(NisseService& other)
{
    if (running || other.running)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::NisseService::swap: move failed. Can't move a service once it has started.");
    }

    using std::swap;
    swap(running,           other.running);
    swap(shutDownNext,      other.shutDownNext);
    swap(eventBase,         other.eventBase);
    swap(handlers,          other.handlers);
    swap(retiredHandlers,   other.retiredHandlers);
    swap(currentHandler,    other.currentHandler);
}

void NisseService::start(double check)
{
    std::cout << "Nisse Started\n";
    running = true;
    while (!shutDownNext)
    {
        std::cout << "Nisse Loop\n";
        runLoop(check);
        purgeRetiredHandlers();
    }
    running = false;
    std::cout << "Nisse Stopped\n";
}

void NisseService::flagShutDown()
{
    shutDownNext = true;
    if (event_base_loopbreak(eventBase.get()) != 0)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::NisseService::flagShutDown: event_base_loopbreak(): Failed");
    }
}

void NisseService::runLoop(double check)
{
    int seconds   = static_cast<int>(check);
    int microSecs = (check - seconds) * 1'000'000;
    static const TimeVal ten_sec{seconds, microSecs};

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

void NisseService::purgeRetiredHandlers()
{
    for (auto const& key: retiredHandlers)
    {
        handlers.erase(key);
    }
    retiredHandlers.clear();
}

void NisseService::delHandler(NisseHandler* oldHandler)
{
    retiredHandlers.emplace_back(oldHandler);
}

void NisseService::addTimer(double timeOut, std::function<void()>&& action)
{
    addHandler<TimerHandler>(std::move(timeOut), std::move(action));
}

void NisseService::setCurrentHandler(NisseHandler* current)
{
    if (current != nullptr)
    {
        currentService = this;
        currentHandler = current;
    }
    else
    {
        currentService = nullptr;
        currentHandler = nullptr;
    }
}

#ifdef COVERAGE_TEST
/*
 * This code is only compiled into the unit tests for code coverage purposes
 * It is not part of the live code.
 */
#include "NisseService.tpp"
#include "test/Action.h"
template void ThorsAnvil::Nisse::Core::Service::NisseService::listenOn<Action>(int);
template void ThorsAnvil::Nisse::Core::Service::NisseService::listenOn<ActionUnReg>(int);
template NisseHandler& ThorsAnvil::Nisse::Core::Service::NisseService::addHandler<Action, ThorsAnvil::Socket::DataSocket>(ThorsAnvil::Socket::DataSocket&&);
template NisseHandler& ThorsAnvil::Nisse::Core::Service::NisseService::addHandler<ActionUnReg, ThorsAnvil::Socket::DataSocket>(ThorsAnvil::Socket::DataSocket&&);
#endif
