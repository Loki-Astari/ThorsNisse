#include "SocketStream.h"

using namespace ThorsAnvil::Socket;

ISocketStream::ISocketStreamBuffer::ISocketStreamBuffer(DataSocket& stream, NoDataAction noAvailableData, std::vector<char>&& bufData, char const* currentStart, char const* currentEnd)
    : stream(stream)
    , noAvailableData(noAvailableData)
    , buffer(std::move(bufData))
{
    char* newStart = const_cast<char*>(currentStart);
    char* newEnd   = const_cast<char*>(currentEnd);
    if (newStart == nullptr || newEnd == nullptr)
    {
        newStart = &buffer[0];
        newEnd   = &buffer[0];
    }

    setg(&buffer[0], newStart, newEnd);
}

ISocketStream::ISocketStreamBuffer::ISocketStreamBuffer(ISocketStreamBuffer&& move) noexcept
    : stream(move.stream)
    , noAvailableData(move.noAvailableData)
    , buffer(std::move(move.buffer))
{
    setg(move.eback(), move.gptr(), move.egptr());
    move.setg(nullptr, nullptr, nullptr);
}

ISocketStream::ISocketStreamBuffer::int_type ISocketStream::ISocketStreamBuffer::underflow()
{
    /*
     * Ensures that at least one character is available in the input area by updating the pointers to the input area (if needed)
     * and reading more data in from the input sequence (if applicable).
     * Returns the value of that character (converted to int_type with Traits::to_int_type(c)) on success or Traits::eof() on failure.
     * The function may update gptr, egptr and eback pointers to define the location of newly loaded data (if any).
     * On failure, the function ensures that either gptr() == nullptr or gptr() == egptr.
     * The base class version of the function does nothing. The derived classes may override this function to allow updates to the get area in the case of exhaustion.
     */

    while (gptr() == egptr())
    {
        bool    moreData;
        size_t  count;
        std::tie(moreData, count) = stream.getMessageData(&buffer[0], buffer.size());
        if (moreData && count == 0)
        {
            noAvailableData();
        }
        else if (count != 0)
        {
            setg(&buffer[0], &buffer[0], &buffer[count]);
        }
        else if (!moreData)
        {
            break;
        }
    }
    return (gptr() == egptr()) ? traits::eof() : traits::to_int_type(*gptr());
}

std::streamsize ISocketStream::ISocketStreamBuffer::xsgetn(char_type* dest, std::streamsize count)
{
    /*
     * Reads count characters from the input sequence and stores them into a character array pointed to by s.
     * The characters are read as if by repeated calls to sbumpc().
     * That is, if less than count characters are immediately available, the function calls uflow() to provide more until traits::eof() is returned.
     * Classes derived from std::basic_streambuf are permitted to provide more efficient implementations of this function.
     */

    std::streamsize currentBufferSize = egptr() - gptr();
    std::streamsize movedCharacter    = std::min(count, currentBufferSize);
    std::copy(gptr(), gptr() + movedCharacter, dest);

    dest  += movedCharacter;
    count -= movedCharacter;

    while (count > 0)
    {
        bool    moreData;
        size_t  dataRead;
        std::tie(moreData, dataRead) = stream.getMessageData(dest, count);
        if (moreData && dataRead == 0)
        {
            noAvailableData();
        }
        else if (!moreData)
        {
            break;
        }
        dest  += dataRead;
        count -= dataRead;
        movedCharacter += dataRead;
    }
    return movedCharacter;
}

// ------------------------

ISocketStream::ISocketStream(DataSocket& stream, NoDataAction noAvailableData, std::vector<char>&& bufData, char const* currentStart, char const* currentEnd)
    : std::istream(nullptr)
    , buffer(stream, noAvailableData, std::move(bufData), currentStart, currentEnd)
{
    std::istream::rdbuf(&buffer);
}

ISocketStream::ISocketStream(ISocketStream&& move) noexcept
    : std::istream(nullptr)
    , buffer(std::move(move.buffer))
{
    std::istream::rdbuf(&buffer);
}

// ------------------------

OSocketStream::OSocketStream(NoDataAction noAvailableData)
    : std::ostream(nullptr)
    , noAvailableData(noAvailableData)
{}

OSocketStream::OSocketStream(OSocketStream&& move) noexcept
    : std::ostream(nullptr)
    , noAvailableData(move.noAvailableData)
{}
