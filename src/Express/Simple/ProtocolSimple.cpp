#include "ProtocolSimple.h"

using namespace ThorsAnvil::Nisse::Protocol::Simple;

std::string const ReadMessageHandler::failSizeMessage       = "Failed: Reading Size Object";
std::string const ReadMessageHandler::failIncompleteMessage = "Failed: Size OK. But message incomplete";
std::string const WriteMessageHandler::messageSuffix        = "-> 200 OK Replied";

ReadMessageHandler::ReadMessageHandler(Core::Service::Server& parent, Core::Socket::DataSocket&& socket)
    : HandlerNonSuspendable(parent, std::move(socket), EV_READ)
    , readSizeObject(0)
    , readBuffer(0)
{}

short ReadMessageHandler::eventActivate(Core::Service::LibSocketId /*sockId*/, short /*eventType*/)
{
    bool more;
    std::size_t read;
    if (readSizeObject != sizeof(readSizeObject))
    {
        std::tie(more, read) = stream.getMessageData(reinterpret_cast<char*>(&bufferSize), sizeof(bufferSize), readSizeObject);
        readSizeObject += read;
        if (readSizeObject != sizeof(readSizeObject))
        {
            if (!more)
            {
                // Did not get a size object
                // And the stream was closed.
                std::string fail = failSizeMessage;
                moveHandler<WriteMessageHandler>(std::move(stream), std::move(fail), false);
                return 0;
            }
            // We have not received all of the size object
            // Return for now. When more data arrives we will
            // try and read more when this function is re-called.
            return EV_READ;
        }
        buffer.resize(bufferSize);
    }
    std::tie(more, read) = stream.getMessageData(&buffer[0], bufferSize, readBuffer);
    readBuffer += read;
    if (readBuffer != bufferSize)
    {
        if (!more)
        {
            // We did not get the full message.
            // There is no more data on the buffer.
            // We are going to give up and drop the
            std::string fail = failIncompleteMessage;
            moveHandler<WriteMessageHandler>(std::move(stream), std::move(fail), false);
            return 0;
        }
        // We have not received all of the message
        // Return for now. When more data arrives we will
        // try and read more when this function is re-called.
        return EV_READ;
    }
    moveHandler<WriteMessageHandler>(std::move(stream), std::move(buffer), true);
    return 0;
}

WriteMessageHandler::WriteMessageHandler(Core::Service::Server& parent, Core::Socket::DataSocket&& socket, std::string const& m, bool ok)
    : HandlerNonSuspendable(parent, std::move(socket), EV_WRITE)
    , writeSizeObject(0)
    , writeBuffer(0)
    , message(m)
{
    if (ok)
    {
        message += messageSuffix;
    }
}

short WriteMessageHandler::eventActivate(Core::Service::LibSocketId /*sockId*/, short /*eventType*/)
{
    bool        more;
    std::size_t written;
    if (writeSizeObject != sizeof(writeSizeObject))
    {
        std::size_t bufferSize = message.size();
        std::tie(more, written) = stream.putMessageData(reinterpret_cast<char*>(&bufferSize), sizeof(bufferSize), writeSizeObject);
        writeSizeObject += written;
        if (writeSizeObject != sizeof(writeSizeObject))
        {
            if (!more)
            {
                dropHandler();
                return 0;
            }
            return EV_WRITE;
        }
    }
    std::tie(more, written) = stream.putMessageData(message.c_str(), message.size(), writeBuffer);
    writeBuffer += written;
    if (writeBuffer != message.size())
    {
        if (!more)
        {
            dropHandler();
            return 0;
        }
        return EV_WRITE;
    }
    dropHandler();
    return 0;
}
