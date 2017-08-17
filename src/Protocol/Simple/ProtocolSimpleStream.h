#ifndef THORSANVIL_NISSE_PROTOCOL_SIMPLE_PROTOCOL_SIMPLE_STREAM_H
#define THORSANVIL_NISSE_PROTOCOL_SIMPLE_PROTOCOL_SIMPLE_STREAM_H

#include "ThorsNisseCoreService/NisseHandler.h"
#include "ThorsNisseCoreService/CoRoutine.h"
#include "ThorsNisseCoreSocket/SocketStream.h"
#include <istream>
#include <ostream>
#include <string>
#include <memory>

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace Protocol
        {
            namespace Simple
            {

using CoRoutine = ThorsAnvil::Nisse::Core::Service::Context<short>::pull_type;
using Yield     = ThorsAnvil::Nisse::Core::Service::Context<short>::push_type;

class Message
{
    public:
        std::size_t     size;
        std::string     message;
    public:
        Message() = default;
        Message(Message const& move)
            : size(move.size)
            , message(move.message)
        {}
        Message(Message&& move)
            : size(std::move(move.size))
            , message(std::move(move.message))
        {}
        friend std::istream& operator>>(std::istream& stream, Message& info)
        {
            bool goodRead = false;
            if (stream.read(reinterpret_cast<char*>(&info.size), sizeof(info.size)) && stream.gcount() == sizeof(info.size))
            {
                info.message.resize(info.size);
                if (stream.read(reinterpret_cast<char*>(&info.message[0]), info.size) && static_cast<std::size_t>(stream.gcount()) == info.size)
                {
                    goodRead = true;
                }
            }
            if (!goodRead)
            {
                stream.setstate(std::ios::failbit);
            }
            return stream;
        }
        friend std::ostream& operator<<(std::ostream& stream, Message& info)
        {
            info.size = info.message.size();
            if (stream.write(reinterpret_cast<char*>(&info.size), sizeof(info.size)))
            {
                stream.write(&info.message[0], info.size);
            }
            return stream;
        }
};

class ReadMessageStreamHandler: public Core::Service::NisseHandler
{
    private:
        CoRoutine       worker;
    public:
        ReadMessageStreamHandler(Core::Service::Server& parent, Core::Socket::DataSocket&& socket);
        virtual short eventActivate(Core::Service::LibSocketId sockId, short eventType) override;
    public:
        static std::string const failToReadMessage;
};

class WriteMessageStreamHandler: public Core::Service::NisseHandler
{
    private:
        CoRoutine       worker;
    public:
        WriteMessageStreamHandler(Core::Service::Server& parent, Core::Socket::DataSocket&& socket, Message&& message);
        WriteMessageStreamHandler(Core::Service::Server& parent, Core::Socket::DataSocket&& socket, Message const& message);
        ~WriteMessageStreamHandler();
        virtual short eventActivate(Core::Service::LibSocketId sockId, short eventType) override;
    public:
        static std::string const messageSuffix;
};

            }
        }
    }
}

#endif
