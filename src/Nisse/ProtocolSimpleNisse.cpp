#include "ProtocolSimpleNisse.h"
#include "NisseService.h"

using namespace ThorsAnvil::Nisse;

DataHandlerReadMessage::DataHandlerReadMessage(NisseService& parent, LibEventBase* base, ThorsAnvil::Socket::DataSocket&& so)
    : NisseHandler(parent, base, so.getSocketId(), EV_READ)
    , socket(std::move(so))
    , readSizeObject(0)
    , readBuffer(0)
{}

void DataHandlerReadMessage::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    if (readSizeObject != sizeof(readSizeObject))
    {
        readSizeObject = socket.getMessageData(reinterpret_cast<char*>(&bufferSize), sizeof(bufferSize), readSizeObject);
        if (readSizeObject != sizeof(readSizeObject))
        {
            return;
        }
        buffer.resize(bufferSize);
    }
    readBuffer = socket.getMessageData(&buffer[0], bufferSize, readBuffer);
    if (readBuffer != bufferSize)
    {
        return;
    }
    moveHandler<DataHandlerWriteMessage>(std::move(socket), buffer);
}

DataHandlerWriteMessage::DataHandlerWriteMessage(NisseService& parent, LibEventBase* base, ThorsAnvil::Socket::DataSocket&& so, std::string const& m)
    : NisseHandler(parent, base, so.getSocketId(), EV_WRITE)
    , socket(std::move(so))
    , writeSizeObject(0)
    , writeBuffer(0)
    , message(m)
{
    message += " -> OK";
}

void DataHandlerWriteMessage::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    if (writeSizeObject != sizeof(writeSizeObject))
    {
        std::size_t bufferSize = message.size();
        writeSizeObject = socket.putMessageData(reinterpret_cast<char*>(&bufferSize), sizeof(bufferSize), writeSizeObject);
        if (writeSizeObject != sizeof(writeSizeObject))
        {
            return;
        }
    }
    writeBuffer = socket.putMessageData(message.c_str(), message.size(), writeBuffer);
    if (writeBuffer != message.size())
    {
        return;
    }
    dropHandler();
}
