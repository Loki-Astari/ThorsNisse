#include "Handler.h"
#include "ServerHandler.h"

using namespace ThorsAnvil::Nisse::Core::Service;

TimerHandler::TimerHandler(Server& parent, double timeOut, std::function<void()>&& action)
    : HandlerNonSuspendable(parent, -1, EV_PERSIST, timeOut)
    , action(std::move(action))
{}

short TimerHandler::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    action();
    return 0;
}

#ifdef COVERAGE_TEST
/*
 * This code is only compiled into the unit tests for code coverage purposes
 * It is not part of the live code.
 */
#include "ServerHandler.tpp"
#include "test/Action.h"
#include "ThorsNisseCoreSocket/Socket.h"

template ThorsAnvil::Nisse::Core::Service::ServerHandler<Action, void>::ServerHandler(ThorsAnvil::Nisse::Core::Service::Server&, ThorsAnvil::Nisse::Core::Socket::ServerSocket&&);
template ThorsAnvil::Nisse::Core::Service::ServerHandler<ActionUnReg, void>::ServerHandler(ThorsAnvil::Nisse::Core::Service::Server&, ThorsAnvil::Nisse::Core::Socket::ServerSocket&&);
template ThorsAnvil::Nisse::Core::Service::ServerHandler<TestHandler, std::tuple<bool, bool, bool> >::ServerHandler(ThorsAnvil::Nisse::Core::Service::Server&, ThorsAnvil::Nisse::Core::Socket::ServerSocket&&, std::tuple<bool, bool, bool>&);
template ThorsAnvil::Nisse::Core::Service::ServerHandler<InHandlerTest, std::tuple<bool, std::function<void (ThorsAnvil::Nisse::Core::Service::Server&)> > >::ServerHandler(ThorsAnvil::Nisse::Core::Service::Server&, ThorsAnvil::Nisse::Core::Socket::ServerSocket&&, std::tuple<bool, std::function<void (ThorsAnvil::Nisse::Core::Service::Server&)> >&);
#endif
