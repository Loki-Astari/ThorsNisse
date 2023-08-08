#include "NonBlockingPrepareStatement.h"
#include "NonBlockingMySQLConnection.h"
#include "StreamCloser.h"
#include "ThorsNisseCoreService/Service.h"
#include "ThorsMySQL/PrepareStatement.h"

using namespace ThorsAnvil::Nisse::Core::SQL;

class MySQLPrepareHandler: public ThorsAnvil::Nisse::Core::Service::HandlerSuspendable<StreamCloser<NonBlockingMySQLConnection>>
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
            : HandlerSuspendable(service, make_StreamCloser(connection), EV_READ | EV_WRITE, EV_WRITE)
            , connection(connection)
            , parent(parent)
            , nbStream(nbStream)
            , statement(statement)
        {}

        virtual bool eventActivateNonBlocking() override
        {
            ThorsAnvil::DB::Access::Lib::YieldSetter   setter(connection, [&parent = *this](){parent.suspend(EV_READ);}, [&parent = *this](){parent.suspend(EV_WRITE);});
            parent.createProxy(nbStream, statement);
            return true;
        }
};

class MySQLExecuteHandler: public ThorsAnvil::Nisse::Core::Service::HandlerSuspendable<StreamCloser<NonBlockingMySQLConnection>>
{
    NonBlockingMySQLConnection&         connection;
    NonBlockingPrepareStatement&        parent;

    public:
        MySQLExecuteHandler(ThorsAnvil::Nisse::Core::Service::Server& service,
                            NonBlockingMySQLConnection& connection,
                            NonBlockingPrepareStatement& parent)
            : HandlerSuspendable(service, make_StreamCloser(connection), EV_READ | EV_WRITE, EV_WRITE)
            , connection(connection)
            , parent(parent)
        {}

        virtual bool eventActivateNonBlocking() override
        {
            ThorsAnvil::DB::Access::Lib::YieldSetter   setter(connection,[&parent = *this](){parent.suspend(EV_READ);}, [&parent = *this](){parent.suspend(EV_WRITE);});
            parent.executePrepare();
            return true;
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
    prepareStatement.reset(new ThorsAnvil::DB::MySQL::PrepareStatement(nbStream, statement));
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
