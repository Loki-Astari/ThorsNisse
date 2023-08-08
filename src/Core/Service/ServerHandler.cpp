#include "Service.h"

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
