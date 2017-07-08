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

class ISocketStream: public std::istream
{
    struct ISocketStreamBuffer: public std::streambuf
    {
        typedef std::streambuf::traits_type traits;
        typedef traits::int_type            int_type;
        typedef traits::char_type           char_type;

        DataSocket&             stream;
        NoDataAction            noAvailableData;
        std::vector<char>       buffer;

        ISocketStreamBuffer(DataSocket& stream, NoDataAction noAvailableData, std::vector<char>&& bufData, char const* currentStart, char const* currentEnd);
        ISocketStreamBuffer(ISocketStreamBuffer&& move) noexcept;
        virtual int_type        underflow() override;
        virtual std::streamsize xsgetn(char_type* s, std::streamsize count) override;
    };

    ISocketStreamBuffer buffer;

    public:
        ISocketStream(DataSocket& stream, NoDataAction noAvailableData, std::vector<char>&& bufData = std::vector<char>(4000), char const* currentStart = nullptr, char const* currentEnd = nullptr);
        ISocketStream(ISocketStream&& move) noexcept;
};

class OSocketStream: public std::ostream
{
    NoDataAction    noAvailableData;
    public:
        OSocketStream(NoDataAction noAvailableData);
        OSocketStream(OSocketStream&& move) noexcept;
};

    }
}

#endif
