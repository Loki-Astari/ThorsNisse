#include "ProtocolSimpleStreamNisse.h"
#include "NisseService.h"

using namespace ThorsAnvil::Nisse::ProtocolSimple;

ReadMessageStreamHandler::ReadMessageStreamHandler(NisseService& parent, LibEventBase* base, ThorsAnvil::Socket::DataSocket&& so)
    : NisseHandler(parent, base, so.getSocketId(), EV_READ)
    , worker([&parent = *this, socket = std::move(so)](Yield& yield) mutable
                {
                    Socket::ISocketStream   stream(socket, [&yield](){yield();});
                    Message                 message;
                    stream >> message;
                    message.message += " -> OK";
                    parent.moveHandler<WriteMessageStreamHandler>(std::move(socket), std::move(message));
                }
            )
{}

void ReadMessageStreamHandler::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    worker();
}

WriteMessageStreamHandler::WriteMessageStreamHandler(NisseService& parent, LibEventBase* base, ThorsAnvil::Socket::DataSocket&& so, Message&& message)
    : NisseHandler(parent, base, so.getSocketId(), EV_WRITE)
    , socket(std::move(so))
    , writeSizeObject(0)
    , writeBuffer(0)
    , message(std::move(message))
{}

void WriteMessageStreamHandler::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    bool        more;
    if (writeSizeObject != sizeof(writeSizeObject))
    {
        std::size_t bufferSize = message.message.size();
        std::tie(more, writeSizeObject) = socket.putMessageData(reinterpret_cast<char*>(&bufferSize), sizeof(bufferSize), writeSizeObject);
        if (!more)
        {
            dropHandler();
            return;
        }
        if (writeSizeObject != sizeof(writeSizeObject))
        {
            return;
        }
    }
    std::tie(more, writeBuffer) = socket.putMessageData(message.message.c_str(), message.message.size(), writeBuffer);
    if (!more)
    {
        dropHandler();
        return;
    }
    if (writeBuffer != message.message.size())
    {
        return;
    }
    dropHandler();
}
