#include "ProtocolSimple.h"
#include "ThorsNisse/NisseService.h"

using namespace ThorsAnvil::Nisse::ProtocolSimple;

std::string const ReadMessageHandler::failSizeMessage       = "Failed: Reading Size Object";
std::string const ReadMessageHandler::failIncompleteMessage = "Failed: Size OK. But message incomplete";

ReadMessageHandler::ReadMessageHandler(NisseService& parent, ThorsAnvil::Socket::DataSocket&& so)
    : NisseHandler(parent, so.getSocketId(), EV_READ)
    , socket(std::move(so))
    , readSizeObject(0)
    , readBuffer(0)
{}

void ReadMessageHandler::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    bool more;
    std::size_t read;
    if (readSizeObject != sizeof(readSizeObject))
    {
        std::tie(more, read) = socket.getMessageData(reinterpret_cast<char*>(&bufferSize), sizeof(bufferSize), readSizeObject);
        readSizeObject += read;
        if (readSizeObject != sizeof(readSizeObject))
        {
            if (!more)
            {
                // Did not get a size object
                // And the stream was closed.
                std::string fail = failSizeMessage;
                moveHandler<WriteMessageHandler>(std::move(socket), std::move(fail), false);
            }
            // We have not received all of the size object
            // Return for now. When more data arrives we will
            // try and read more when this function is re-called.
            return;
        }
        buffer.resize(bufferSize);
    }
    std::tie(more, read) = socket.getMessageData(&buffer[0], bufferSize, readBuffer);
    readBuffer += read;
    if (readBuffer != bufferSize)
    {
        if (!more)
        {
            // We did not get the full message.
            // There is no more data on the buffer.
            // We are going to give up and drop the
            std::string fail = failIncompleteMessage;
            moveHandler<WriteMessageHandler>(std::move(socket), std::move(fail), false);
        }
        // We have not received all of the message
        // Return for now. When more data arrives we will
        // try and read more when this function is re-called.
        return;
    }
    moveHandler<WriteMessageHandler>(std::move(socket), std::move(buffer));
}

WriteMessageHandler::WriteMessageHandler(NisseService& parent, ThorsAnvil::Socket::DataSocket&& so, std::string const& m)
    : NisseHandler(parent, so.getSocketId(), EV_WRITE)
    , socket(std::move(so))
    , writeSizeObject(0)
    , writeBuffer(0)
    , message(m)
{
    if (ok)
    {
        message += " -> OK";
    }
}

void WriteMessageHandler::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    bool        more;
    std::size_t written;
    if (writeSizeObject != sizeof(writeSizeObject))
    {
        std::size_t bufferSize = message.size();
        std::tie(more, written) = socket.putMessageData(reinterpret_cast<char*>(&bufferSize), sizeof(bufferSize), writeSizeObject);
        writeSizeObject += written;
        if (writeSizeObject != sizeof(writeSizeObject))
        {
            if (!more)
            {
                dropHandler();
            }
            return;
        }
    }
    std::tie(more, written) = socket.putMessageData(message.c_str(), message.size(), writeBuffer);
    writeBuffer += written;
    if (writeBuffer != message.size())
    {
        if (!more)
        {
            dropHandler();
        }
        return;
    }
    dropHandler();
}
