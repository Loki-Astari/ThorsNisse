#include "ProtocolSimpleStream.h"

using namespace ThorsAnvil::Nisse::Protocol::Simple;

std::string const ReadMessageStreamHandler::failToReadMessage = "Message Read Failed";
std::string const WriteMessageStreamHandler::messageSuffix    = " -> OK <-";

ReadMessageStreamHandler::ReadMessageStreamHandler(Core::Service::Server& parent, Core::Socket::DataSocket&& so)
    : NisseHandler(parent, so.getSocketId(), EV_READ)
    , worker([&parent = *this, socket = std::move(so)](Yield& yield) mutable
      {
          Core::Socket::ISocketStream   stream(socket, [&yield](){yield(EV_READ);}, [](){});
          yield(EV_READ);
          Message                 message;
          if (!(stream >> message))
          {
              message.message = failToReadMessage;
          }
          parent.moveHandler<WriteMessageStreamHandler>(std::move(socket), std::move(message));
      })
{}

short ReadMessageStreamHandler::eventActivate(Core::Service::LibSocketId /*sockId*/, short /*eventType*/)
{
    return (!worker())
        ? 0     // The co-routine has completed.
        : worker.get();
}

WriteMessageStreamHandler::WriteMessageStreamHandler(Core::Service::Server& parent, Core::Socket::DataSocket&& so, Message&& ms)
    : NisseHandler(parent, so.getSocketId(), EV_WRITE)
    , worker([&parent = *this, socket = std::move(so), message = std::move(ms)](Yield& yield) mutable
      {
          Core::Socket::OSocketStream   stream(socket, [&yield](){yield(EV_WRITE);}, [](){});
          yield(0);
          message.message += messageSuffix;
          stream << message;
          parent.dropHandler();
      })
{}

WriteMessageStreamHandler::WriteMessageStreamHandler(Core::Service::Server& parent, Core::Socket::DataSocket&& so, Message const& ms)
    : NisseHandler(parent, so.getSocketId(), EV_WRITE)
    , worker([&parent = *this, socket = std::move(so), message(ms)](Yield& yield) mutable
      {
          Core::Socket::OSocketStream   stream(socket, [&yield](){yield(EV_WRITE);}, [](){});
          yield(0);
          message.message += messageSuffix;
          stream << message;
          parent.dropHandler();
      })
{}

WriteMessageStreamHandler::~WriteMessageStreamHandler()
{}

short WriteMessageStreamHandler::eventActivate(Core::Service::LibSocketId /*sockId*/, short /*eventType*/)
{
    return (!worker())
        ? 0     // The co-routine has completed.
        : worker.get();
}

#ifdef COVERAGE_TEST
/*
 * This code is only compiled into the unit tests for code coverage purposes
 * It is not part of the live code.
 */
#include "ThorsNisseCoreService/Server.h"
#include "ThorsNisseCoreService/Server.tpp"
#include "ThorsNisseCoreService/NisseHandler.tpp"
#include "ProtocolSimple.h"
template void ThorsAnvil::Nisse::Core::Service::Server::listenOn<ReadMessageStreamHandler>(int);
template void ThorsAnvil::Nisse::Core::Service::Server::listenOn<WriteMessageStreamHandler, Message>(int, Message&);
template ThorsAnvil::Nisse::Core::Service::ServerHandler<ReadMessageHandler, void>::ServerHandler(ThorsAnvil::Nisse::Core::Service::Server&, ThorsAnvil::Nisse::Core::Socket::ServerSocket&&);
template void ThorsAnvil::Nisse::Core::Service::NisseHandler::moveHandler<WriteMessageStreamHandler, ThorsAnvil::Nisse::Core::Socket::DataSocket, Message>(ThorsAnvil::Nisse::Core::Socket::DataSocket&&, Message&&);
template void ThorsAnvil::Nisse::Core::Service::NisseHandler::moveHandler<WriteMessageHandler, ThorsAnvil::Nisse::Core::Socket::DataSocket, std::string, bool>(ThorsAnvil::Nisse::Core::Socket::DataSocket&&, std::string&&, bool&&);
template ThorsAnvil::Nisse::Core::Service::NisseHandler& ThorsAnvil::Nisse::Core::Service::Server::addHandler<WriteMessageHandler, ThorsAnvil::Nisse::Core::Socket::DataSocket, std::string, bool>(ThorsAnvil::Nisse::Core::Socket::DataSocket&&, std::string&&, bool&&);
template ThorsAnvil::Nisse::Core::Service::ServerHandler<WriteMessageHandler, std::string>::ServerHandler(ThorsAnvil::Nisse::Core::Service::Server&, ThorsAnvil::Nisse::Core::Socket::ServerSocket&&, std::string&);
#endif
