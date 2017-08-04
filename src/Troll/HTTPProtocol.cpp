#include "HTTPProtocol.h"
#include "HttpScanner.h"
#include "ThorsNisse/NisseService.h"

using namespace ThorsAnvil::Nisse::ProtocolHTTP;

class DevNullStreamBuf: public std::streambuf
{
    private:
        typedef std::streambuf::traits_type traits;
        typedef traits::int_type            int_type;
        typedef traits::char_type           char_type;

        std::size_t written;

    protected:
        virtual std::streamsize xsputn(char_type const* /*s*/,std::streamsize count) override
        {
            written += count;
            return count;
        }

        virtual std::streampos seekoff(std::streamoff /*off*/,
                                       std::ios_base::seekdir /*way*/,
                                       std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override
        {
            ((void)which);
            return written;
        }
    public:
        DevNullStreamBuf()
            : written(0)
        {}
};

ReadRequestHandler::ReadRequestHandler(NisseService& parent, ThorsAnvil::Socket::DataSocket&& so, Binder const& binder)
    : NisseHandler(parent, so.getSocketId(), EV_READ)
    , flusher(nullptr)
    , socket(std::move(so))
    , binder(binder)
    , buffer(bufferLen)
    , worker([ &parent = *this
             , &socket = this->socket
             , &scanner = this->scanner
             , &binder = this->binder
             , &buffer = this->buffer
             ](Yield& yield) mutable
        {
            yield(EV_READ);
            while (!scanner.data.messageComplete)
            {
                bool                more;
                std::size_t         recved;
                std::tie(more, recved) = socket.getMessageData(&buffer[0], bufferLen, 0);
                scanner.scan(&buffer[0], recved);

                if (scanner.data.messageComplete || !more)
                {
                    break;
                }
                yield(EV_READ);
            }
            URI const     uri(scanner.data.headers.get("Host"), std::move(scanner.data.uri));
            Action const& action(binder.find(scanner.data.method, uri.host, uri.path));
            Socket::ISocketStream   input(socket, [&yield](){yield(EV_READ);}, [](){}, std::move(buffer), scanner.data.bodyBegin, scanner.data.bodyEnd);
            Socket::OSocketStream   output(socket, [&yield](){yield(EV_WRITE);}, [&parent](){parent.flushing();});
            DevNullStreamBuf        devNullBuffer;
            if (scanner.data.method == Method::Head)
            {
                output.rdbuf(&devNullBuffer);
            }

            yield(EV_READ | EV_WRITE);
            Request       request(scanner.data.method, URI(std::move(uri)), scanner.data.headers, input);
            Response      response(parent, socket, output);

            action(request, response);
        })
{}

short ReadRequestHandler::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    if (!worker())
    {
        dropHandler();
        return 0;
    }
    return worker.get();
}
