#include "HTTPProtocol.h"
#include "HttpScanner.h"
#include "ThorsNisse/NisseService.h"

using namespace ThorsAnvil::Nisse::ProtocolHTTP;

ReadRequestHandler::ReadRequestHandler(NisseService& parent, ThorsAnvil::Socket::DataSocket&& so, Binder const& binder)
    : NisseHandler(parent, so.getSocketId(), EV_READ)
    , socket(std::move(so))
    , binder(binder)
    , buffer(bufferLen)
    , worker([ &socket = this->socket
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
        })
{}

short ReadRequestHandler::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    if (!worker())
    {
        if (scanner.data.messageComplete)
        {
            requestComplete(std::move(socket),
                            binder,
                            scanner.data.method,
                            std::move(scanner.data.uri),
                            std::move(scanner.data.headers),
                            std::move(buffer),
                            scanner.data.bodyBegin, scanner.data.bodyEnd
                           );
        }
        else
        {
            dropHandler();
        }
        return 0;
    }
    return worker.get();
}

void ReadRequestHandler::requestComplete(
                     DataSocket&&    socket,
                     Binder const&   binder,
                     Method          method,
                     std::string&&   uri,
                     Headers&&       headers,
                     std::vector<char>&& buffer,
                     char const* bodyBegin, char const* bodyEnd)
{
    moveHandler<WriteResponseHandler>(std::move(socket),
                                      binder,
                                      method,
                                      std::move(uri),
                                      std::move(headers),
                                      std::move(buffer),
                                      bodyBegin, bodyEnd
                                     );
}

// ------------------

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

WriteResponseHandler::WriteResponseHandler(NisseService& parent,
                                           ThorsAnvil::Socket::DataSocket&& so,
                                           Binder const& binder,
                                           Method methodParam,
                                           std::string&& uriParam,
                                           Headers&& headersParam,
                                           std::vector<char>&& bufferParam,
                                           char const* bodyBeginParam,
                                           char const* bodyEndParam)
    : NisseHandler(parent, so.getSocketId(), EV_WRITE | EV_READ)
    , flusher(nullptr)
    , worker([ &parent      = *this,
                socket      = std::move(so),
               &binder      = binder,
                method      = methodParam,
                uriParam    = std::move(uriParam),
                headers     = std::move(headersParam),
                buffer      = std::move(bufferParam),
                bodyBegin   = bodyBeginParam,
                bodyEnd     = bodyEndParam
             ](Yield& yield) mutable
                {
                    URI const     uri(headers.get("Host"), std::move(uriParam));
                    Action const& action(binder.find(method, uri.host, uri.path));
                    Socket::ISocketStream   input(socket, [&yield](){yield(EV_READ);}, [](){}, std::move(buffer), bodyBegin, bodyEnd);
                    Socket::OSocketStream   output(socket, [&yield](){yield(EV_WRITE);}, [&parent](){parent.flushing();});
                    DevNullStreamBuf        devNullBuffer;
                    if (method == Method::Head)
                    {
                        output.rdbuf(&devNullBuffer);
                    }

                    Request       request(method, URI(std::move(uri)), headers, input);
                    Response      response(parent, socket, output);
                    yield(EV_READ | EV_WRITE);
                    action(request, response);
                }
            )
{}

short WriteResponseHandler::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    worker();
    if (!worker)
    {
        dropHandler();
        return 0;
    }
    return worker.get();
}

#ifdef COVERAGE_TEST
/*
 * This code is only compiled into the unit tests for code coverage purposes
 * It is not part of the live code.
 */
#include "ThorsNisse/NisseHandler.h"
#include "ThorsNisse/NisseService.tpp"
#include "ThorsNisse/NisseHandler.tpp"
#include "Types.h"
template void ThorsAnvil::Nisse::NisseHandler::moveHandler
    <WriteResponseHandler,
     ThorsAnvil::Socket::DataSocket,
     Binder const&,
     Method,
     std::string,
     Headers,
     std::vector<char>,
     char const*,
     char const*>
    (ThorsAnvil::Socket::DataSocket&&,
     Binder const&,
     Method&&,
     std::string&&,
     Headers&&,
     std::vector<char>&&,
     char const*&&,
     char const*&&);


template void ThorsAnvil::Nisse::NisseService::addHandler<WriteResponseHandler, ThorsAnvil::Socket::DataSocket, Binder const&, Method, std::string, Headers, std::vector<char>, char const*, char const*>(ThorsAnvil::Socket::DataSocket&&, Binder const&, Method&&, std::string&&, Headers&&, std::vector<char>&&, char const*&&, char const*&&);
#endif
