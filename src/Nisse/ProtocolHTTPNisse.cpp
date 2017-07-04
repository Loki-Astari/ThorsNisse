#include "ProtocolHTTPNisse.h"
#include "NisseService.h"

using namespace ThorsAnvil::Nisse;

HTTPHandlerAccept::HTTPHandlerAccept(NisseService& parent, LibEventBase* base, ThorsAnvil::Socket::DataSocket&& so)
    : NisseHandler(parent, base, so.getSocketId())
    , socket(std::move(so))
{}

void HTTPHandlerAccept::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
}
