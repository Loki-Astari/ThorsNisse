#ifndef THORSANVIL_SIMPLE_STREAM_THOR_STREAM_H
#define THORSANVIL_SIMPLE_STREAM_THOR_STREAM_H

#include "Socket.h"
#include <istream>
#include <vector>

namespace ThorsAnvil
{
    namespace Socket
    {

using Notifier = std::function<void()>;

inline void noActionNotifier(){}

class SocketStreamBuffer: public std::streambuf
{
    private:
        typedef std::streambuf::traits_type traits;
        typedef traits::int_type            int_type;
        typedef traits::char_type           char_type;

        DataSocket&             stream;
        Notifier                noAvailableData;
        Notifier                flushing;
        std::vector<char>       buffer;
        bool                    closeSocketOnDestruction;

    public:
        virtual ~SocketStreamBuffer() override;
        SocketStreamBuffer(DataSocket& stream,
                           Notifier noAvailableData, Notifier flushing, bool closeSocketOnDestruction = true,
                           std::vector<char>&& bufData = std::vector<char>(4000),
                           char const* currentStart = nullptr, char const* currentEnd = nullptr);
        SocketStreamBuffer(SocketStreamBuffer&& move) noexcept;

    protected:
        virtual int_type        underflow() override;
        virtual std::streamsize xsgetn(char_type* s, std::streamsize count) override;

        virtual int_type        overflow(int_type ch = traits::eof()) override;
        virtual std::streamsize xsputn(char_type const* s,std::streamsize count) override;
    private:
        std::streamsize writeToStream(char_type const* source, std::streamsize count);
        std::streamsize readFromStream(char_type* dest, std::streamsize count, bool fill = true);
};

class ISocketStream: public std::istream
{
    SocketStreamBuffer buffer;

    public:
        ISocketStream(DataSocket& stream,
                      Notifier noAvailableData, Notifier flushing, bool closeSocketOnDestruction,
                      std::vector<char>&& bufData, char const* currentStart, char const* currentEnd);
        ISocketStream(DataSocket& stream,
                      Notifier noAvailableData, Notifier flushing, bool closeSocketOnDestruction = true);
        ISocketStream(DataSocket& stream, bool closeSocketOnDestruction = true);
        ISocketStream(ISocketStream&& move) noexcept;
};

class OSocketStream: public std::ostream
{
    SocketStreamBuffer buffer;

    public:
        OSocketStream(DataSocket& stream,
                      Notifier noAvailableData, Notifier flushing, bool closeSocketOnDestruction = true);
        OSocketStream(DataSocket& stream, bool closeSocketOnDestruction = true);
        OSocketStream(OSocketStream&& move) noexcept;
};

    }
}

#endif
