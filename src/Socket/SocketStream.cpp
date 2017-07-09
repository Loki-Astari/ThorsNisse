#include "SocketStream.h"

using namespace ThorsAnvil::Socket;

SocketStreamBuffer::SocketStreamBuffer(DataSocket& stream, NoDataAction noAvailableData, std::vector<char>&& bufData, char const* currentStart, char const* currentEnd)
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
    setp(&buffer[0], &buffer[buffer.size() - 1]);
}

SocketStreamBuffer::SocketStreamBuffer(SocketStreamBuffer&& move) noexcept
    : stream(move.stream)
    , noAvailableData(move.noAvailableData)
    , buffer(std::move(move.buffer))
{
    move.setg(nullptr, nullptr, nullptr);
    move.setp(nullptr, nullptr);
}

SocketStreamBuffer::int_type SocketStreamBuffer::underflow()
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

std::streamsize SocketStreamBuffer::xsgetn(char_type* dest, std::streamsize count)
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

SocketStreamBuffer::int_type SocketStreamBuffer::overflow(int_type ch)
{
    /*
     * Ensures that there is space at the put area for at least one character by saving some initial subsequence of
     * characters starting at pbase() to the output sequence and updating the pointers to the put area (if needed).
     * If ch is not Traits::eof() (i.e. Traits::eq_int_type(ch, Traits::eof()) != true),
     *     it is either put to the put area or directly saved to the output sequence.
     * The function may update pptr, epptr and pbase pointers to define the location to write more data.
     * On failure, the function ensures that either pptr() == nullptr or pptr() == epptr.
     * The base class version of the function does nothing. The derived classes may override this function to allow
     * updates to the put area in the case of exhaustion.
     */

    /* Note: When we set the "put" pointers we delibrately leave an extra space that is not buffer.
     * When overflow is called the normal buffer is used up, but there is an extra space in the real
     * underlying buffer that we can use.
     *
     * So: *pptr = ch; // will never fail.
     */
    *pptr() = ch;
    pbump(1);

    std::streamsize toWrite = pptr() - pbase();
    std::streamsize written = 0;
    while (toWrite != written)
    {
        bool        moreSpace;
        std::size_t count;
        std::tie(moreSpace, count) = stream.putMessageData(pbase(), pptr() - pbase(), written);
        if (moreSpace && count == 0)
        {
            noAvailableData();
        }
        else if (moreSpace)
        {
            written += count;
        }
        else
        {
            return traits::eof();
        }
    }
    setp(&buffer[0], &buffer[buffer.size() - 1]);
    return written;
}

std::streamsize SocketStreamBuffer::xsputn(char_type const* source, std::streamsize count)
{
    std::streamsize written = 0;
    if (overflow() != traits::eof())
    {
        while (count != written)
        {
            bool        moreSpace;
            std::size_t count;
            std::tie(moreSpace, count) = stream.putMessageData(source, count, written);
            if (moreSpace && count == 0)
            {
                noAvailableData();
            }
            else if (moreSpace)
            {
                written += count;
            }
            else
            {
                break;
            }
        }
    }
    return written;
}
// ------------------------

ISocketStream::ISocketStream(DataSocket& stream, NoDataAction noAvailableData)
    : std::istream(nullptr)
    , buffer(stream, noAvailableData)
{
    std::istream::rdbuf(&buffer);
}

ISocketStream::ISocketStream(DataSocket& stream, NoDataAction noAvailableData, std::vector<char>&& bufData, char const* currentStart, char const* currentEnd)
    : std::istream(nullptr)
    , buffer(stream, noAvailableData, std::move(bufData), currentStart, currentEnd)
{
    rdbuf(&buffer);
}

ISocketStream::ISocketStream(ISocketStream&& move) noexcept
    : std::istream(nullptr)
    , buffer(std::move(move.buffer))
{
    rdbuf(&buffer);
}

// ------------------------

OSocketStream::OSocketStream(DataSocket& stream, NoDataAction noAvailableData)
    : std::ostream(nullptr)
    , buffer(stream, noAvailableData)
{
    rdbuf(&buffer);
}

OSocketStream::OSocketStream(OSocketStream&& move) noexcept
    : std::ostream(nullptr)
    , buffer(std::move(move.buffer))
{
    rdbuf(&buffer);
}
