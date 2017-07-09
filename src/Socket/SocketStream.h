#ifndef THORSANVIL_SIMPLE_STREAM_THOR_STREAM_H
#define THORSANVIL_SIMPLE_STREAM_THOR_STREAM_H

#include "Socket.h"
#include <istream>
#include <vector>

namespace ThorsAnvil
{
    namespace Socket
    {

using NoDataAction = std::function<void()>;

class SocketStreamBuffer: public std::streambuf
{
    private:
        typedef std::streambuf::traits_type traits;
        typedef traits::int_type            int_type;
        typedef traits::char_type           char_type;

        DataSocket&             stream;
        NoDataAction            noAvailableData;
        std::vector<char>       buffer;

    public:
        virtual ~SocketStreamBuffer() override;
        SocketStreamBuffer(DataSocket& stream, NoDataAction noAvailableData, std::vector<char>&& bufData = std::vector<char>(4000), char const* currentStart = nullptr, char const* currentEnd = nullptr);
        SocketStreamBuffer(SocketStreamBuffer&& move) noexcept;

    protected:
        virtual int_type        underflow() override;
        virtual std::streamsize xsgetn(char_type* s, std::streamsize count) override;

        virtual int_type        overflow(int_type ch = traits::eof()) override;
        virtual std::streamsize xsputn(char_type const* s,std::streamsize count) override;
};

class ISocketStream: public std::istream
{
    SocketStreamBuffer buffer;

    public:
        ISocketStream(DataSocket& stream, NoDataAction noAvailableData, std::vector<char>&& bufData, char const* currentStart, char const* currentEnd);
        ISocketStream(DataSocket& stream, NoDataAction noAvailableData);
        ISocketStream(ISocketStream&& move) noexcept;
};

class OSocketStream: public std::ostream
{
    SocketStreamBuffer buffer;

    public:
        OSocketStream(DataSocket& stream, NoDataAction noAvailableData);
        OSocketStream(OSocketStream&& move) noexcept;
};

    }
}

#endif
