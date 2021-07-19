#include "ConnectionNonBlocking.h"
#include "StreamCloser.h"
#include "ThorsNisseCoreService/Server.h"
#include "ThorsNisseCoreService/Handler.h"

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace Core
        {
            namespace SQL
            {

class MySQLConnectionHandler: public Service::HandlerSuspendable<StreamCloser<DBMySQL::MySQLStream>>
{
    ConnectionNonBlocking&          connection;
    DBMySQL::MySQLStream&           stream;
    std::string                     username;
    std::string                     password;
    std::string                     database;
    DB::Options const&              options;
    public:
        MySQLConnectionHandler(Service::Server& service,
                               ConnectionNonBlocking& connection,
                               DBMySQL::MySQLStream& stream,
                               std::string const& username,
                               std::string const& password,
                               std::string const& database,
                               DB::Options const& options)
            : HandlerSuspendable(service, make_StreamCloser(stream), EV_READ | EV_WRITE, EV_WRITE)
            , connection(connection)
            , stream(stream)
            , username(username)
            , password(password)
            , database(database)
            , options(options)
        {}
        virtual bool eventActivateNonBlocking() override
        {
            // MIY TODO DBSQL::Lib::YieldSetter   setter(stream, [&parent = *this](){parent.suspend(EV_READ);}, [&parent = *this](){parent.suspend(EV_WRITE);});
            stream.setYield([&parent = *this](){parent.suspend(EV_READ);}, [&parent = *this](){parent.suspend(EV_WRITE);});
            connection.doConectToServer(username, password, database, options);
            stream.setYield([](){}, [](){});
            return true;
        }
};

            }
        }
    }
}

using namespace ThorsAnvil::Nisse::Core::SQL;

ConnectionNonBlocking::ConnectionNonBlocking(
        DBMySQL::MySQLStream& stream,
        std::string const& username,
        std::string const& password,
        std::string const& database,
        DB::Options const& options,
        DBMySQL::ConectReader& packageReader,
        DBMySQL::ConectWriter& packageWriter
)
    : Connection(username, password, database, options, packageReader, packageWriter)
{
    if (Service::Server::inHandler())
    {
        auto& service = Service::Server::getCurrentHandler();
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
        DB::Options const& options
)
{
    conectToServer(username, password, database, options);
}
