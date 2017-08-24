#include "NonBlockingPrepareStatement.h"
#include "NonBlockingMySQLConnection.h"
#include "ThorsNisseCoreService/Server.h"
#include "ThorsNisseCoreService/Handler.h"
#include "ThorMySQL/PrepareStatement.h"

using namespace ThorsAnvil::Nisse::Core::SQL;

class MySQLPrepareHandler: public ThorsAnvil::Nisse::Core::Service::HandlerSuspendable
{
    NonBlockingMySQLConnection&         connection;
    NonBlockingPrepareStatement&        parent;
    ConnectionNonBlocking&              nbStream;
    std::string                         statement;
    public:
        MySQLPrepareHandler(ThorsAnvil::Nisse::Core::Service::Server& service,
                            NonBlockingMySQLConnection& connection,
                            NonBlockingPrepareStatement& parent,
                            ConnectionNonBlocking& nbStream,
                            std::string const& statement)
            : HandlerSuspendable(service, connection.getSocketId(), EV_READ | EV_WRITE, EV_WRITE)
            , connection(connection)
            , parent(parent)
            , nbStream(nbStream)
            , statement(statement)
        {}

        virtual void eventActivateNonBlocking() override
        {
            ThorsAnvil::SQL::Lib::YieldSetter   setter(connection, [&yield = *(this->yield)](){yield(EV_READ);}, [&yield = *(this->yield)](){yield(EV_WRITE);});
            parent.createProxy(nbStream, statement);
        }
};

class MySQLExecuteHandler: public ThorsAnvil::Nisse::Core::Service::HandlerSuspendable
{
    NonBlockingMySQLConnection&         connection;
    NonBlockingPrepareStatement&        parent;

    public:
        MySQLExecuteHandler(ThorsAnvil::Nisse::Core::Service::Server& service,
                            NonBlockingMySQLConnection& connection,
                            NonBlockingPrepareStatement& parent)
            : HandlerSuspendable(service, connection.getSocketId(), EV_READ | EV_WRITE, EV_WRITE)
            , connection(connection)
            , parent(parent)
        {}

        virtual void eventActivateNonBlocking() override
        {
            ThorsAnvil::SQL::Lib::YieldSetter   setter(connection,[&yield = *(this->yield)](){yield(EV_READ);}, [&yield = *(this->yield)](){yield(EV_WRITE);});
            parent.executePrepare();
        }
};

NonBlockingPrepareStatement::NonBlockingPrepareStatement(NonBlockingMySQLConnection& connection, ConnectionNonBlocking& nbStream, std::string const& statement)
    : prepareStatement(nullptr)
    , connection(connection)
{
    if (Service::Server::inHandler())
    {
        auto& service = Service::Server::getCurrentHandler();
        service.transferHandler<MySQLPrepareHandler>(connection, *this, nbStream, statement);
    }
    else
    {
        createProxy(nbStream, statement);
    }
}
void NonBlockingPrepareStatement::createProxy(ConnectionNonBlocking& nbStream, std::string const& statement)
{
    prepareStatement.reset(new ThorsAnvil::MySQL::PrepareStatement(nbStream, statement));
}

void NonBlockingPrepareStatement::doExecute()
{
    if (! Service::Server::inHandler())
    {
        throw std::runtime_error("ThorsAnvil::Nisse::Core::SQL::NonBlockingPrepareStatement::doExecute: Can only use this prepare inside a Handler");
    }
    auto& service = Service::Server::getCurrentHandler();
    service.transferHandler<MySQLExecuteHandler>(connection, *this);
}
