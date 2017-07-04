#ifndef THORSANVIL_NISSE_PROTOCOL_HTTP_NISSE_H
#define THORSANVIL_NISSE_PROTOCOL_HTTP_NISSE_H

#include "NisseHandler.h"
#include <boost/coroutine/asymmetric_coroutine.hpp>
#include <map>
#include <vector>

namespace ThorsAnvil
{
    namespace Nisse
    {

class HTTPHandlerAccept: public NisseHandler
{
    private:
        ThorsAnvil::Socket::DataSocket                  socket;
        std::string                                     method;
        std::string                                     uri;
        std::string                                     version;
        std::map<std::string, std::vector<std::string>> headers;

        using CoRoutine = boost::coroutines::asymmetric_coroutine<void>::pull_type;
        using Yield     = boost::coroutines::asymmetric_coroutine<void>::push_type;

        CoRoutine   parseHeader;
    public:
        HTTPHandlerAccept(NisseService& parent, LibEventBase* base, ThorsAnvil::Socket::DataSocket&& socket);
        void headerParser(Yield& yield);
        virtual void eventActivate(LibSocketId sockId, short eventType) override;

};

    }
}

#endif
