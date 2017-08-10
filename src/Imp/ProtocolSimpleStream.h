#ifndef THORSANVIL_NISSE_PROTOCOL_SIMPLE_STREAM_NISSE_H
#define THORSANVIL_NISSE_PROTOCOL_SIMPLE_STREAM_NISSE_H

#include "ImpConfig.h"
#include "ThorsNisse/NisseHandler.h"
#include "ThorsNisseSocket/SocketStream.h"
#if BOOST_COROUTINE_VERSION == 1 || BOOST_COROUTINE_VERSION == 2
#include <boost/coroutine/all.hpp>
#elif BOOST_COROUTINE_VERSION == 3
#include <boost/coroutine2/all.hpp>
#else
#error "Unknown Co-Routine Version"
#endif

namespace ThorsAnvil
{
    namespace CoRoutine
    {
#if BOOST_COROUTINE_VERSION == 1
        template<typename R>
        struct Context
        {
            using pull_type = typename boost::coroutines::coroutine<R()>;
            using push_type = typename pull_type::caller_type;
        };
#elif BOOST_COROUTINE_VERSION == 2
        template<typename R>
        using Context = typename boost::coroutines::asymmetric_coroutine<R>;
#elif BOOST_COROUTINE_VERSION == 3
        template<typename R>
        using Context = typename boost::coroutines2::coroutine<R>;
#endif
    }
}
#include <ios>

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace ProtocolSimple
        {

using CoRoutine = ThorsAnvil::CoRoutine::Context<short>::pull_type;
using Yield     = ThorsAnvil::CoRoutine::Context<short>::push_type;

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

class ReadMessageStreamHandler: public NisseHandler
{
    private:
        CoRoutine       worker;
    public:
        ReadMessageStreamHandler(NisseService& parent, ThorsAnvil::Socket::DataSocket&& socket);
        virtual short eventActivate(LibSocketId sockId, short eventType) override;
    public:
        static std::string const failToReadMessage;
};

class WriteMessageStreamHandler: public NisseHandler
{
    private:
        CoRoutine       worker;
    public:
        WriteMessageStreamHandler(NisseService& parent, ThorsAnvil::Socket::DataSocket&& socket, Message&& message);
        WriteMessageStreamHandler(NisseService& parent, ThorsAnvil::Socket::DataSocket&& socket, Message const& message);
        ~WriteMessageStreamHandler();
        virtual short eventActivate(LibSocketId sockId, short eventType) override;
    public:
        static std::string const messageSuffix;
};

        }
    }
}

#endif
