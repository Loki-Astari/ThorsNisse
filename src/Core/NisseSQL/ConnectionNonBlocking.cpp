#include "ConnectionNonBlocking.h"
#include "ThorsNisseCoreService/NisseService.h"
#include "ThorsNisseCoreService/NisseHandler.h"

namespace ThorsAnvil
{
    namespace NisseSQL
    {

class MySQLConnectionHandler: public Nisse::Core::Service::NisseHandler
{
    CoRoutine               worker;
    public:
        MySQLConnectionHandler(Nisse::Core::Service::NisseService& service,
                               ConnectionNonBlocking& connection,
                               ThorsAnvil::MySQL::MySQLStream& stream,
                               std::string const& username,
                               std::string const& password,
                               std::string const& database,
                               ThorsAnvil::SQL::Options const& options)
            : NisseHandler(service, stream.getSocketId(), EV_READ | EV_WRITE)
            , worker([&connection, &stream, &username, &password, &database, &options](Yield& yield)
                {
                    yield(EV_WRITE);
                    stream.setYield([&yield](){yield(EV_READ);}, [&yield](){yield(EV_WRITE);});
                    connection.doConectToServer(username, password, database, options);
                    stream.setYield([](){}, [](){});
                })
        {
        }
        virtual short eventActivate(Nisse::Core::Service::LibSocketId /*sockId*/, short /*eventType*/)
        {
            if (!worker())
            {
                dropHandler();
                return 0;
            }
            return worker.get();
        }
};

    }
}

using namespace ThorsAnvil::NisseSQL;

ConnectionNonBlocking::ConnectionNonBlocking(
        ThorsAnvil::MySQL::MySQLStream& stream,
        std::string const& username,
        std::string const& password,
        std::string const& database,
        ThorsAnvil::SQL::Options const& options,
        ThorsAnvil::MySQL::ConectReader& packageReader,
        ThorsAnvil::MySQL::ConectWriter& packageWriter
)
    : Connection(username, password, database, options, packageReader, packageWriter)
{
    if (Nisse::Core::Service::NisseService::inHandler())
    {
        auto& service = Nisse::Core::Service::NisseService::getCurrentHandler();
        service.transferHandler<MySQLConnectionHandler>(*this, stream, username, password, database, options);
    }
    else
    {
        doConectToServer(username, password, database, options);
    }
}

void ConnectionNonBlocking::doConectToServer(
        std::string const& username,
        std::string const& password,
        std::string const& database,
        ThorsAnvil::SQL::Options const& options
)
{
    conectToServer(username, password, database, options);
}
