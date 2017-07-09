#ifndef THORSANVIL_NISSE_PROTOCOL_SIMPLE_STREAM_NISSE_H
#define THORSANVIL_NISSE_PROTOCOL_SIMPLE_STREAM_NISSE_H

#include "NisseHandler.h"
#include "ThorsNisseSocket/SocketStream.h"
#include <boost/coroutine/asymmetric_coroutine.hpp>

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace ProtocolSimple
        {

using CoRoutine = boost::coroutines::asymmetric_coroutine<void>::pull_type;
using Yield     = boost::coroutines::asymmetric_coroutine<void>::push_type;

class Message
{
    public:
        std::size_t             size;
        std::string             message;
    public:
        friend std::istream& operator>>(std::istream& stream, Message& info)
        {
            if (stream.read(reinterpret_cast<char*>(&info.size), sizeof(info.size)))
            {
                info.message.resize(info.size);
                stream.read(reinterpret_cast<char*>(&info.message[0]), info.size);
            }
            return stream;
        }
};

class ReadMessageStreamHandler: public NisseHandler
{
    private:
        CoRoutine                           worker;
    public:
        ReadMessageStreamHandler(NisseService& parent, LibEventBase* base, ThorsAnvil::Socket::DataSocket&& socket);
        virtual void eventActivate(LibSocketId sockId, short eventType) override;
};

class WriteMessageStreamHandler: public NisseHandler
{
    private:
        ThorsAnvil::Socket::DataSocket      socket;
        std::size_t                         writeSizeObject;
        std::size_t                         writeBuffer;
        Message                             message;
    public:
        WriteMessageStreamHandler(NisseService& parent, LibEventBase* base, ThorsAnvil::Socket::DataSocket&& socket, Message&& message);
        virtual void eventActivate(LibSocketId sockId, short eventType) override;
};

        }
    }
}

#endif