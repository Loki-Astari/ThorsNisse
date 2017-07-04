#include "ProtocolHTTPNisse.h"
#include "NisseService.h"

using namespace ThorsAnvil::Nisse;

HTTPHandlerAccept::HTTPHandlerAccept(NisseService& parent, LibEventBase* base, ThorsAnvil::Socket::DataSocket&& so)
    : NisseHandler(parent, base, so.getSocketId())
    , socket(std::move(so))
    , parseHeader([parent = this](Yield& yield){parent->headerParser(yield);})
{}

/*
        std::string                                     method;
        std::string                                     uri;
        std::string                                     version
        std::map<std::string, std::vector<std::string>> headers;
*/
void HTTPHandlerAccept::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    parseHeader();
}

void HTTPHandlerAccept::headerParser(Yield& yield)
{
    while (true)
    {
        try
        {
            yield();
        }
        catch (...)
        {}
    }
}
