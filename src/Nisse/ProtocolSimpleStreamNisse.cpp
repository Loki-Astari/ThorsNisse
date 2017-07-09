#include "ProtocolSimpleStreamNisse.h"
#include "NisseService.h"

using namespace ThorsAnvil::Nisse::ProtocolSimple;

ReadMessageStreamHandler::ReadMessageStreamHandler(NisseService& parent, LibEventBase* base, ThorsAnvil::Socket::DataSocket&& so)
    : NisseHandler(parent, base, so.getSocketId(), EV_READ)
    , worker([&parent = *this, socket = std::move(so)](Yield& yield) mutable
                {
                    Socket::ISocketStream   stream(socket, [&yield](){yield();}, [](){});
                    Message                 message;
                    stream >> message;
                    parent.moveHandler<WriteMessageStreamHandler>(std::move(socket), std::move(message));
                }
            )
{}

void ReadMessageStreamHandler::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    worker();
}

WriteMessageStreamHandler::WriteMessageStreamHandler(NisseService& parent, LibEventBase* base, ThorsAnvil::Socket::DataSocket&& so, Message&& ms)
    : NisseHandler(parent, base, so.getSocketId(), EV_WRITE)
    , worker([&parent = *this, socket = std::move(so), message = std::move(ms)](Yield& yield) mutable
                {
                    Socket::OSocketStream   stream(socket, [&yield](){yield();}, [](){});
                    message.message += " -> OK <-";
                    stream << message;
                    parent.dropHandler();
                }
            )
{}

void WriteMessageStreamHandler::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    worker();
}
