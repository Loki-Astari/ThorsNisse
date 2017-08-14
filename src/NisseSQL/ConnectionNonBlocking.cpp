#include "ConnectionNonBlocking.h"
#include <iostream>

namespace ThorsAnvil
{
    namespace NisseSQL
    {

class MySQLConnectionHandler: public ThorsAnvil::Nisse::NisseHandler
{
    CoRoutine               worker;
    public:
        MySQLConnectionHandler(ThorsAnvil::Nisse::NisseService& service,
                               ConnectionNonBlocking& connection,
                               ThorsAnvil::MySQL::MySQLStream& stream,
                               std::string const& username,
                               std::string const& password,
                               std::string const& database,
                               ThorsAnvil::SQL::Options const& options)
            : NisseHandler(service, stream.getSocketId(), EV_READ | EV_WRITE)
            , worker([&connection, &stream, &username, &password, &database, &options](Yield& yield)
                {
                    std::cerr << "MySQLConnectionHandler: Worker\n";
                    yield(EV_WRITE);
                    std::cerr << "MySQLConnectionHandler: Worder: Setting Yield\n";
                    ThorsAnvil::MySQL::YieldSetter  setter(stream, [&yield](){yield(EV_READ);}, [&yield](){yield(EV_WRITE);});
                    std::cerr << "MySQLConnectionHandler: Worder: Setting Connecting to server\n";
                    connection.doConectToServer(username, password, database, options);
                    std::cerr << "MySQLConnectionHandler: Worder: DONE\n";
                })
        {
            std::cerr << "MySQLConnectionHandler: CONSTRUCTOR\n";
        }
        virtual short eventActivate(ThorsAnvil::Nisse::LibSocketId /*sockId*/, short /*eventType*/)
        {
            std::cerr << "We Can Write To DB\n";
            if (!worker())
            {
                std::cerr << "event Activate Finished\n";
                dropHandler();
                return 0;
            }
            short x = worker.get();
            std::cerr << "DB connection Blocked: " << (x & EV_READ ? "READ" : "NR") << " : " << (x & EV_WRITE ? "WRITE" : "NW") << "\n";
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
    , stream(stream)
{
std::cerr << "ConnectionNonBlocking::ConnectionNonBlocking\n";
    auto& service = ThorsAnvil::Nisse::NisseService::getCurrentHandler();
std::cerr << "ConnectionNonBlocking: Service: " << &service << "\n";
    service.transferHandler<MySQLConnectionHandler>(*this, stream, username, password, database, options);
std::cerr << "Transfer Returned\n";
}

void ConnectionNonBlocking::doConectToServer(
        std::string const& username,
        std::string const& password,
        std::string const& database,
        ThorsAnvil::SQL::Options const& options
)
{
    std::cerr << "doConectToServer: " << username << " : " << password << " : " << database << "\n";
    conectToServer(username, password, database, options);
}
