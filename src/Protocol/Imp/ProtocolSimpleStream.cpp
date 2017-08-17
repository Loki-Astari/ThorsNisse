#include "ProtocolSimpleStream.h"

using namespace ThorsAnvil::Nisse::ProtocolSimple;

std::string const ReadMessageStreamHandler::failToReadMessage = "Message Read Failed";
std::string const WriteMessageStreamHandler::messageSuffix    = " -> OK <-";

ReadMessageStreamHandler::ReadMessageStreamHandler(NisseService& parent, ThorsAnvil::Socket::DataSocket&& so)
    : NisseHandler(parent, so.getSocketId(), EV_READ)
    , worker([&parent = *this, socket = std::move(so)](Yield& yield) mutable
      {
          Socket::ISocketStream   stream(socket, [&yield](){yield(EV_READ);}, [](){});
          yield(EV_READ);
          Message                 message;
          if (!(stream >> message))
          {
              message.message = failToReadMessage;
          }
          parent.moveHandler<WriteMessageStreamHandler>(std::move(socket), std::move(message));
      })
{}

short ReadMessageStreamHandler::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    return (!worker())
        ? 0     // The co-routine has completed.
        : worker.get();
}

WriteMessageStreamHandler::WriteMessageStreamHandler(NisseService& parent, ThorsAnvil::Socket::DataSocket&& so, Message&& ms)
    : NisseHandler(parent, so.getSocketId(), EV_WRITE)
    , worker([&parent = *this, socket = std::move(so), message = std::move(ms)](Yield& yield) mutable
      {
          Socket::OSocketStream   stream(socket, [&yield](){yield(EV_WRITE);}, [](){});
          yield(0);
          message.message += messageSuffix;
          stream << message;
          parent.dropHandler();
      })
{}

WriteMessageStreamHandler::WriteMessageStreamHandler(NisseService& parent, ThorsAnvil::Socket::DataSocket&& so, Message const& ms)
    : NisseHandler(parent, so.getSocketId(), EV_WRITE)
    , worker([&parent = *this, socket = std::move(so), message(ms)](Yield& yield) mutable
      {
          Socket::OSocketStream   stream(socket, [&yield](){yield(EV_WRITE);}, [](){});
          yield(0);
          message.message += messageSuffix;
          stream << message;
          parent.dropHandler();
      })
{}

WriteMessageStreamHandler::~WriteMessageStreamHandler()
{}

short WriteMessageStreamHandler::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
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
#include "ThorsNisse/NisseService.h"
#include "ThorsNisse/NisseService.tpp"
#include "ThorsNisse/NisseHandler.tpp"
#include "ProtocolSimple.h"
template void ThorsAnvil::Nisse::NisseService::listenOn<ReadMessageStreamHandler>(int);
template void ThorsAnvil::Nisse::NisseService::listenOn<WriteMessageStreamHandler, Message>(int, Message&);
template ThorsAnvil::Nisse::ServerHandler<ReadMessageHandler, void>::ServerHandler(ThorsAnvil::Nisse::NisseService&, ThorsAnvil::Socket::ServerSocket&&);
template void ThorsAnvil::Nisse::NisseHandler::moveHandler<WriteMessageStreamHandler, ThorsAnvil::Socket::DataSocket, Message>(ThorsAnvil::Socket::DataSocket&&, Message&&);
template void ThorsAnvil::Nisse::NisseHandler::moveHandler<WriteMessageHandler, ThorsAnvil::Socket::DataSocket, std::string, bool>(ThorsAnvil::Socket::DataSocket&&, std::string&&, bool&&);
template ThorsAnvil::Nisse::NisseHandler& ThorsAnvil::Nisse::NisseService::addHandler<WriteMessageHandler, ThorsAnvil::Socket::DataSocket, std::string, bool>(ThorsAnvil::Socket::DataSocket&&, std::string&&, bool&&);
template ThorsAnvil::Nisse::ServerHandler<WriteMessageHandler, std::string>::ServerHandler(ThorsAnvil::Nisse::NisseService&, ThorsAnvil::Socket::ServerSocket&&, std::string&);
#endif
