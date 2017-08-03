#include "ProtocolSimple.h"
#include "ThorsNisse/NisseService.h"

using namespace ThorsAnvil::Nisse::ProtocolSimple;

std::string const ReadMessageHandler::failSizeMessage       = "Failed: Reading Size Object";
std::string const ReadMessageHandler::failIncompleteMessage = "Failed: Size OK. But message incomplete";
std::string const WriteMessageHandler::messageSuffix        = "-> 200 OK Replied";

ReadMessageHandler::ReadMessageHandler(NisseService& parent, ThorsAnvil::Socket::DataSocket&& so)
    : NisseHandler(parent, so.getSocketId(), EV_READ)
    , socket(std::move(so))
    , readSizeObject(0)
    , readBuffer(0)
{}

short ReadMessageHandler::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
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
                return 0;
            }
            // We have not received all of the size object
            // Return for now. When more data arrives we will
            // try and read more when this function is re-called.
            return EV_READ;
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
            return 0;
        }
        // We have not received all of the message
        // Return for now. When more data arrives we will
        // try and read more when this function is re-called.
        return EV_READ;
    }
    moveHandler<WriteMessageHandler>(std::move(socket), std::move(buffer), true);
    return 0;
}

WriteMessageHandler::WriteMessageHandler(NisseService& parent, ThorsAnvil::Socket::DataSocket&& so, std::string const& m, bool ok)
    : NisseHandler(parent, so.getSocketId(), EV_WRITE | EV_PERSIST)
    , socket(std::move(so))
    , writeSizeObject(0)
    , writeBuffer(0)
    , message(m)
{
    if (ok)
    {
        message += messageSuffix;
    }
}

short WriteMessageHandler::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
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
            return 0;
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
        return 0;
    }
    dropHandler();
    return 0;
}

#ifdef COVERAGE_TEST
/*
 * This code is only compiled into the unit tests for code coverage purposes
 * It is not part of the live code.
 */
#include "ThorsNisse/NisseService.tpp"
template void ThorsAnvil::Nisse::NisseService::listenOn<ReadMessageHandler>(int);
template void ThorsAnvil::Nisse::NisseService::listenOn<WriteMessageHandler, std::string>(int, std::string&);
#endif
