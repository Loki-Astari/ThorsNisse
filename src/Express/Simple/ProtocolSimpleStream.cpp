#include "ProtocolSimpleStream.h"
#include "ThorsSocket/SocketStream.h"

using namespace ThorsAnvil::Nisse::Protocol::Simple;

std::string const ReadMessageStreamHandler::failToReadMessage = "Message Read Failed";
std::string const WriteMessageStreamHandler::messageSuffix    = " -> OK <-";

ReadMessageStreamHandler::ReadMessageStreamHandler(Core::Service::Server& parent, ThorsSocket::DataSocket&& socket)
    : HandlerSuspendable(parent, std::move(socket), EV_READ)
{}

bool ReadMessageStreamHandler::eventActivateNonBlocking()
{
    ThorsSocket::IOSocketStream   istream(stream, [&parent = *this](){parent.suspend(EV_READ);}, [](){});
    Message                 message;
    if (!(istream >> message))
    {
        message.message = failToReadMessage;
    }
    moveHandler<WriteMessageStreamHandler>(std::move(stream), std::move(message));
    return false;
}

WriteMessageStreamHandler::WriteMessageStreamHandler(Core::Service::Server& parent, ThorsSocket::DataSocket&& socket, Message&& ms)
    : HandlerSuspendable(parent, std::move(socket), EV_WRITE, 0)
    , message(std::move(ms))
{}

WriteMessageStreamHandler::WriteMessageStreamHandler(Core::Service::Server& parent, ThorsSocket::DataSocket&& socket, Message const& ms)
    : HandlerSuspendable(parent, std::move(socket), EV_WRITE, 0)
    , message(ms)
{}

WriteMessageStreamHandler::~WriteMessageStreamHandler()
{}

bool WriteMessageStreamHandler::eventActivateNonBlocking()
{
    ThorsSocket::IOSocketStream   istream(stream, [](){}, [&parent = *this](){parent.suspend(EV_WRITE);});
    message.message += messageSuffix;
    istream << message;
    return true;
}
