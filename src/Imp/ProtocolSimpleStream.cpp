#include "ProtocolSimpleStream.h"
#include "ThorsNisse/NisseService.h"

using namespace ThorsAnvil::Nisse::ProtocolSimple;

std::string const ReadMessageStreamHandler::failToReadMessage = "Message Read Failed";
std::string const WriteMessageStreamHandler::messageSuffix    = " -> OK <-";

ReadMessageStreamHandler::ReadMessageStreamHandler(NisseService& parent, ThorsAnvil::Socket::DataSocket&& so)
    : NisseHandler(parent, so.getSocketId(), EV_READ)
    , worker([&parent = *this, socket = std::move(so)](Yield& yield) mutable
                {
                    Socket::ISocketStream   stream(socket, [&yield](){yield();}, [](){}, false);
                    yield();
                    Message                 message;
                    if (!(stream >> message))
                    {
                        message.message = failToReadMessage;
                    }
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
                    yield();
                    message.message += messageSuffix;
                    stream << message;
                    parent.dropHandler();
                }
            )
{}

WriteMessageStreamHandler::~WriteMessageStreamHandler()
{}

void WriteMessageStreamHandler::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    worker();
}
