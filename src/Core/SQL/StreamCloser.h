#ifndef THORSANVIL_NISSE_CORE_SQL_STREAM_CLOSER_H
#define THORSANVIL_NISSE_CORE_SQL_STREAM_CLOSER_H

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace Core
        {
            namespace SQL
            {

template<typename Stream>
class StreamCloser
{
    Stream&  stream;
    public:
        StreamCloser(Stream& stream)
            : stream(stream)
        {}
        void close()
        {
            stream.close();
        }
        int getSocketId() const
        {
            return stream.getSocketId();
        }
};

template<typename Stream>
StreamCloser<Stream> make_StreamCloser(Stream& stream)
{
    return StreamCloser<Stream>(stream);
}

            }
        }
    }
}

#endif
