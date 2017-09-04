#include "ProtocolSimpleStream.h"

using namespace ThorsAnvil::Nisse::Protocol::Simple;

std::string const ReadMessageStreamHandler::failToReadMessage = "Message Read Failed";
std::string const WriteMessageStreamHandler::messageSuffix    = " -> OK <-";

ReadMessageStreamHandler::ReadMessageStreamHandler(Core::Service::Server& parent, Core::Socket::DataSocket&& socket)
    : HandlerSuspendable(parent, std::move(socket), EV_READ)
{}

bool ReadMessageStreamHandler::eventActivateNonBlocking()
{
    Core::Socket::ISocketStream   istream(stream, [&parent = *this](){parent.suspend(EV_READ);}, [](){});
    Message                 message;
    if (!(istream >> message))
    {
        message.message = failToReadMessage;
    }
    moveHandler<WriteMessageStreamHandler>(std::move(stream), std::move(message));
    return false;
}

WriteMessageStreamHandler::WriteMessageStreamHandler(Core::Service::Server& parent, Core::Socket::DataSocket&& socket, Message&& ms)
    : HandlerSuspendable(parent, std::move(socket), EV_WRITE, 0)
    , message(std::move(ms))
{}

WriteMessageStreamHandler::WriteMessageStreamHandler(Core::Service::Server& parent, Core::Socket::DataSocket&& socket, Message const& ms)
    : HandlerSuspendable(parent, std::move(socket), EV_WRITE, 0)
    , message(ms)
{}

WriteMessageStreamHandler::~WriteMessageStreamHandler()
{}

bool WriteMessageStreamHandler::eventActivateNonBlocking()
{
    Core::Socket::OSocketStream   istream(stream, [&parent = *this](){parent.suspend(EV_WRITE);}, [](){});
    message.message += messageSuffix;
    istream << message;
    return true;
}

#ifdef COVERAGE_TEST
/*
 * This code is only compiled into the unit tests for code coverage purposes
 * It is not part of the live code.
 */
#include "ThorsNisseCoreService/Server.h"
#include "ThorsNisseCoreService/Server.tpp"
#include "ThorsNisseCoreService/Handler.tpp"
#include "ProtocolSimple.h"
template void ThorsAnvil::Nisse::Core::Service::Server::listenOn<ReadMessageStreamHandler>(ServerConnection const&);
template void ThorsAnvil::Nisse::Core::Service::Server::listenOn<WriteMessageStreamHandler, Message>(ServerConnection const&, Message&);
template ThorsAnvil::Nisse::Core::Service::ServerHandler<ReadMessageHandler, void>::ServerHandler(ThorsAnvil::Nisse::Core::Service::Server&, ThorsAnvil::Nisse::Core::Socket::ServerSocket&&);
template void ThorsAnvil::Nisse::Core::Service::HandlerBase::moveHandler<WriteMessageStreamHandler, ThorsAnvil::Nisse::Core::Socket::DataSocket, Message>(ThorsAnvil::Nisse::Core::Socket::DataSocket&&, Message&&);
template void ThorsAnvil::Nisse::Core::Service::HandlerBase::moveHandler<WriteMessageHandler, ThorsAnvil::Nisse::Core::Socket::DataSocket, std::string, bool>(ThorsAnvil::Nisse::Core::Socket::DataSocket&&, std::string&&, bool&&);
template ThorsAnvil::Nisse::Core::Service::HandlerBase& ThorsAnvil::Nisse::Core::Service::Server::addHandler<WriteMessageHandler, ThorsAnvil::Nisse::Core::Socket::DataSocket, std::string, bool>(ThorsAnvil::Nisse::Core::Socket::DataSocket&&, std::string&&, bool&&);
template ThorsAnvil::Nisse::Core::Service::ServerHandler<WriteMessageHandler, std::string>::ServerHandler(ThorsAnvil::Nisse::Core::Service::Server&, ThorsAnvil::Nisse::Core::Socket::ServerSocket&&, std::string&);
#endif
