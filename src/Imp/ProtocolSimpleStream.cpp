#include "ProtocolSimpleStream.h"
#include "ThorsNisse/NisseService.h"

using namespace ThorsAnvil::Nisse::ProtocolSimple;

ReadMessageStreamHandler::ReadMessageStreamHandler(NisseService& parent, ThorsAnvil::Socket::DataSocket&& so)
    : NisseHandler(parent, so.getSocketId(), EV_READ)
    , worker([&parent = *this, socket = std::move(so)](Yield& yield) mutable
                {
                    Socket::ISocketStream   stream(socket, [&yield](){yield();}, [](){}, false);
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

WriteMessageStreamHandler::WriteMessageStreamHandler(NisseService& parent, ThorsAnvil::Socket::DataSocket&& so, Message&& ms)
    : NisseHandler(parent, so.getSocketId(), EV_WRITE)
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