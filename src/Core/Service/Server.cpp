#include "Server.h"
#include "Handler.h"

#include <iostream>

using namespace ThorsAnvil::Nisse::Core::Service;

EventConfig*  Server::cfg              = nullptr;
Server*       Server::currentService   = nullptr;

Server::Server()
    : running(false)
    , shutDownNext(false)
    , eventBase(event_base_new_with_config(cfg), &event_base_free)
    , currentHandler(nullptr)
{
    if (eventBase == nullptr)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::Server::Server: event_base_new(): Failed");
    }
}

Server::Server(Server&& move)
    : running(false)
    , shutDownNext(false)
    , eventBase(nullptr, &event_base_free)
    , currentHandler(nullptr)
{
    swap(move);
}

Server& Server::operator=(Server&& move)
{
    swap(move);
    return *this;
}

void Server::swap(Server& other)
{
    if (running || other.running)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::Server::swap: move failed. Can't move a service once it has started.");
    }

    using std::swap;
    swap(running,           other.running);
    swap(shutDownNext,      other.shutDownNext);
    swap(eventBase,         other.eventBase);
    swap(handlers,          other.handlers);
    swap(retiredHandlers,   other.retiredHandlers);
    swap(currentHandler,    other.currentHandler);
}

void Server::start(double check)
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

void Server::flagShutDown()
{
    shutDownNext = true;
    if (event_base_loopbreak(eventBase.get()) != 0)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::Server::flagShutDown: event_base_loopbreak(): Failed");
    }
}

void Server::runLoop(double check)
{
    int seconds   = static_cast<int>(check);
    int microSecs = (check - seconds) * 1'000'000;
    static const TimeVal ten_sec{seconds, microSecs};

    if (event_base_loopexit(eventBase.get(), &ten_sec) != 0)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::Server::runLoop: event_base_loopexit(): Failed");
    }
    switch (event_base_dispatch(eventBase.get()))
    {
        case -1:
            throw std::runtime_error("ThorsAnvil::Nisse::Server::runLoop: event_base_dispatch(): Failed");
        case 1:
            throw std::runtime_error("ThorsAnvil::Nisse::Server::runLoop: event_base_dispatch(): No Events");
        case 0:
            break;
        default:
            throw std::runtime_error("ThorsAnvil::Nisse::Server::runLoop: event_base_dispatch(): Unknown Error");
    }
}

void Server::purgeRetiredHandlers()
{
    for (auto const& key: retiredHandlers)
    {
        handlers.erase(key);
    }
    retiredHandlers.clear();
}

void Server::delHandler(HandlerBase* oldHandler)
{
    retiredHandlers.emplace_back(oldHandler);
}

void Server::addTimer(double timeOut, std::function<void()>&& action)
{
    addHandler<TimerHandler>(std::move(timeOut), std::move(action));
}

void Server::setCurrentHandler(HandlerBase* current)
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

// The result of this call is only valid if `inHandler()` returns true.
Server& Server::getCurrentHandler()
{
    return *currentService;
}

bool Server::inHandler()
{
    // If there is a current handler active.
    // And that handler is non-suspendable
    return currentService && currentService->currentHandler && currentService->currentHandler->suspendable();
}
#ifdef COVERAGE_TEST
/*
 * This code is only compiled into the unit tests for code coverage purposes
 * It is not part of the live code.
 */
#include "Server.tpp"
#include "test/Action.h"
template void ThorsAnvil::Nisse::Core::Service::Server::listenOn<Action>(int);
template void ThorsAnvil::Nisse::Core::Service::Server::listenOn<ActionUnReg>(int);
template HandlerBase& ThorsAnvil::Nisse::Core::Service::Server::addHandler<Action, ThorsAnvil::Nisse::Core::Socket::DataSocket>(ThorsAnvil::Nisse::Core::Socket::DataSocket&&);
template HandlerBase& ThorsAnvil::Nisse::Core::Service::Server::addHandler<ActionUnReg, ThorsAnvil::Nisse::Core::Socket::DataSocket>(ThorsAnvil::Nisse::Core::Socket::DataSocket&&);
#endif
