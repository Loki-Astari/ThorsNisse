#include "HTTPProtocol.h"
#include "ThorsNisseSocket/SocketStream.h"

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
    , yield(nullptr)
    , running(false)
    , worker([ &parent = *this
             , socket = std::move(so)
             , &binder
             , &parentYield = this->yield
             ](Yield& yield) mutable
        {
            HttpScanner         scanner;
            std::vector<char>   buffer(bufferLen);
            yield(EV_READ);
            parentYield = &yield;
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
            Socket::ISocketStream   input(socket, [&yield](){yield(EV_READ);}, [](){}, std::move(buffer), scanner.data.bodyBegin, scanner.data.bodyEnd);
            Socket::OSocketStream   output(socket, [&yield](){yield(EV_WRITE);}, [&parent](){parent.flushing();});
            DevNullStreamBuf        devNullBuffer;
            if (scanner.data.method == Method::Head)
            {
                output.rdbuf(&devNullBuffer);
            }

            URI const     uri(scanner.data.headers.get("Host"), std::move(scanner.data.uri));
            Action const& action(binder.find(scanner.data.method, uri.host, uri.path));
            Request       request(scanner.data.method, URI(std::move(uri)), scanner.data.headers, input);
            Response      response(parent, socket, output);

            action(request, response);
        })
{}

void ReadRequestHandler::suspend()
{
    if (!running)
    {
        throw std::runtime_error("Trying to transfer while not running");
    }
    (*yield)(0);
}

struct SetRunning
{
    bool& running;
    SetRunning(bool& running)
        : running(running)
    {
        running = true;
    }
    ~SetRunning()
    {
        running = false;
    }
};

short ReadRequestHandler::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    SetRunning setRunning(running);
    if (!worker())
    {
        dropHandler();
        return 0;
    }
    return worker.get();
}
