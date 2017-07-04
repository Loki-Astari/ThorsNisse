#include "NisseHandler.h"
#include "NisseService.h"

#include <iostream>

using namespace ThorsAnvil::Nisse;

void eventCB(LibSocketId socketId, short eventType, void* event)
{
    NisseHandler& handler = *reinterpret_cast<NisseHandler*>(event);
    handler.eventActivate(socketId, eventType);
}

NisseEvent::NisseEvent(LibEventBase* base, LibSocketId socketId, NisseHandler& parent, short eventType)
    : event(event_new(base, socketId,  eventType | EV_PERSIST, eventCB, &parent))
{
    if (event == nullptr)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::NisseEvent::NisseEvent: event_new(): Failed");
    }
    if (event_add(event, nullptr) != 0)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::NisseEvent::NisseEvent: event_add(): Failed");
    }
}

NisseEvent::~NisseEvent()
{
    event_free(event);
}

void NisseEvent::drop()
{
    if (event_del(event) != 0)
    {
        throw std::runtime_error("ThorsAnvil::Nisse::NisseEvent::drop: event_del(): Failed");
    }
}

NisseHandler::NisseHandler(NisseService& parent, LibEventBase* base, LibSocketId socketId)
    : parent(parent)
    , eventListener(base, socketId, *this, EV_READ)
{}

void NisseHandler::eventActivate(LibSocketId sockId, short eventType)
{
    std::cerr << "Callback made: " << sockId << " For " << eventType << "\n";
}

ServerHandler::ServerHandler(NisseService& parent, LibEventBase* base, ThorsAnvil::Socket::ServerSocket&& so)
    : NisseHandler(parent, base, so.getSocketId())
    , socket(std::move(so))
{}

void ServerHandler::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    ThorsAnvil::Socket::DataSocket accepted = socket.accept();
    parent.addHandler<DataHandlerReadMessage>(std::move(accepted));
}

DataHandlerReadMessage::DataHandlerReadMessage(NisseService& parent, LibEventBase* base, ThorsAnvil::Socket::DataSocket&& so)
    : NisseHandler(parent, base, so.getSocketId())
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
    eventListener.drop();
    parent.delHandler(this);
    //parent.addHandler<DataHandlerWriteMessage>(std::move(accepted));
}
