#include "ProtocolSimpleNisse.h"
#include "NisseService.h"

using namespace ThorsAnvil::Nisse::ProtocolSimple;

ReadMessageHandler::ReadMessageHandler(NisseService& parent, LibEventBase* base, ThorsAnvil::Socket::DataSocket&& so)
    : NisseHandler(parent, base, so.getSocketId(), EV_READ)
    , socket(std::move(so))
    , readSizeObject(0)
    , readBuffer(0)
{}

void ReadMessageHandler::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    bool more;
    if (readSizeObject != sizeof(readSizeObject))
    {
        std::tie(more, readSizeObject) = socket.getMessageData(reinterpret_cast<char*>(&bufferSize), sizeof(bufferSize), readSizeObject);
        if (!more)
        {
            dropHandler();
            return;
        }
        if (readSizeObject != sizeof(readSizeObject))
        {
            return;
        }
        buffer.resize(bufferSize);
    }
    std::tie(more, readBuffer) = socket.getMessageData(&buffer[0], bufferSize, readBuffer);
    if (!more)
    {
        dropHandler();
        return;
    }
    if (readBuffer != bufferSize)
    {
        return;
    }
    moveHandler<WriteMessageHandler>(std::move(socket), buffer);
}

WriteMessageHandler::WriteMessageHandler(NisseService& parent, LibEventBase* base, ThorsAnvil::Socket::DataSocket&& so, std::string const& m)
    : NisseHandler(parent, base, so.getSocketId(), EV_WRITE)
    , socket(std::move(so))
    , writeSizeObject(0)
    , writeBuffer(0)
    , message(m)
{
    message += " -> OK";
}

void WriteMessageHandler::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    bool        more;
    if (writeSizeObject != sizeof(writeSizeObject))
    {
        std::size_t bufferSize = message.size();
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
    std::tie(more, writeBuffer) = socket.putMessageData(message.c_str(), message.size(), writeBuffer);
    if (!more)
    {
        dropHandler();
        return;
    }
    if (writeBuffer != message.size())
    {
        return;
    }
    dropHandler();
}
