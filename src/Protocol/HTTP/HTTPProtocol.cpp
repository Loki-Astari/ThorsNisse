#include "HTTPProtocol.h"
#include "ThorsNisseCoreSocket/SocketStream.h"

using namespace ThorsAnvil::Nisse::Protocol::HTTP;

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

ReadRequestHandler::ReadRequestHandler(Core::Service::Server& parent, Core::Socket::DataSocket&& socket, Binder const& binder)
    : HandlerSuspendable(parent, std::move(socket), EV_READ)
    , binder(binder)
    , flusher(nullptr)
    , running(false)
{}

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

void ReadRequestHandler::eventActivateNonBlocking()
{
    SetRunning          setRunning(running);
    HttpScanner         scanner;
    std::vector<char>   buffer(bufferLen);

    while (!scanner.data.messageComplete)
    {
        bool                more;
        std::size_t         recved;
        std::tie(more, recved) = stream.getMessageData(&buffer[0], bufferLen, 0);
        scanner.scan(&buffer[0], recved);

        if (scanner.data.messageComplete || !more)
        {
            break;
        }
        suspend(EV_READ);
    }
    Core::Socket::ISocketStream   input(stream,  [&parent = *this](){parent.suspend(EV_READ);}, [](){}, std::move(buffer), scanner.data.bodyBegin, scanner.data.bodyEnd);
    Core::Socket::OSocketStream   output(stream, [&parent = *this](){parent.suspend(EV_WRITE);}, [&parent = *this](){parent.flushing();});
    DevNullStreamBuf        devNullBuffer;
    if (scanner.data.method == Method::Head)
    {
        output.rdbuf(&devNullBuffer);
    }

    URI const     uri(scanner.data.headers.get("Host"), std::move(scanner.data.uri));
    Action const& action(binder.find(scanner.data.method, uri.host, uri.path));
    Request       request(scanner.data.method, URI(std::move(uri)), scanner.data.headers, input);
    Response      response(*this, stream, output);

    action(request, response);
}
