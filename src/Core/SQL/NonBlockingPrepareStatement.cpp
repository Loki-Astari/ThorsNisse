#include "NonBlockingPrepareStatement.h"
#include "NonBlockingMySQLConnection.h"
#include "ThorsNisseCoreService/Server.h"
#include "ThorsNisseCoreService/NisseHandler.h"
#include "ThorsNisseCoreService/CoRoutine.h"
#include "ThorMySQL/PrepareStatement.h"

using namespace ThorsAnvil::Nisse::Core::SQL;

using CoRoutine = ThorsAnvil::Nisse::Core::Service::Context<short>::pull_type;
using Yield     = ThorsAnvil::Nisse::Core::Service::Context<short>::push_type;

class MySQLPrepareHandler: public ThorsAnvil::Nisse::Core::Service::NisseHandler
{
    CoRoutine   worker;
    public:
        MySQLPrepareHandler(ThorsAnvil::Nisse::Core::Service::Server& service,
                            NonBlockingMySQLConnection& connection,
                            NonBlockingPrepareStatement& parent,
                            ConnectionNonBlocking& nbStream,
                            std::string const& statement)
            : NisseHandler(service, connection.getSocketId(), EV_READ | EV_WRITE)
            , worker([&connection, &parent, &nbStream, &statement](Yield& yield)
                {
                    yield(EV_WRITE);
                    ThorsAnvil::SQL::Lib::YieldSetter   setter(connection, [&yield](){yield(EV_READ);}, [&yield](){yield(EV_WRITE);});
                    StatmentPIMPL                       result(new ThorsAnvil::MySQL::PrepareStatement(nbStream, statement));
                    parent.setStatement(std::move(result));
                })
        {}

        virtual short eventActivate(ThorsAnvil::Nisse::Core::Service::LibSocketId /*sockId*/, short /*eventType*/) override
        {
            if (!worker())
            {
                dropHandler();
                return 0;
            }
            return worker.get();
        }
};

class MySQLExecuteHandler: public ThorsAnvil::Nisse::Core::Service::NisseHandler
{
    CoRoutine   worker;
    public:
        MySQLExecuteHandler(ThorsAnvil::Nisse::Core::Service::Server& service,
                            NonBlockingMySQLConnection& connection,
                            NonBlockingPrepareStatement& parent)
            : NisseHandler(service, connection.getSocketId(), EV_READ | EV_WRITE)
            , worker([&parent, &connection](Yield& yield)
                {
                    yield(EV_WRITE);
                    ThorsAnvil::SQL::Lib::YieldSetter   setter(connection,[&yield](){yield(EV_READ);}, [&yield](){yield(EV_WRITE);});
                    parent.executePrepare();
                })
        {}

        virtual short eventActivate(ThorsAnvil::Nisse::Core::Service::LibSocketId /*sockId*/, short /*eventType*/) override
        {
            if (!worker())
            {
                dropHandler();
                return 0;
            }
            return worker.get();
        }
};

NonBlockingPrepareStatement::NonBlockingPrepareStatement(NonBlockingMySQLConnection& connection, ConnectionNonBlocking& nbStream, std::string const& statement)
    : prepareStatement(nullptr)
    , connection(connection)
{
    if (!Service::Server::inHandler())
    {
        throw std::runtime_error("ThorsAnvil::Nisse::Core::SQL::NonBlockingPrepareStatement::NonBlockingPrepareStatement: Can only use this prepare inside a NisseHandler");
    }
    auto& service = Service::Server::getCurrentHandler();
    service.transferHandler<MySQLPrepareHandler>(connection, *this, nbStream, statement);
}

void NonBlockingPrepareStatement::doExecute()
{
    if (! Service::Server::inHandler())
    {
        throw std::runtime_error("ThorsAnvil::Nisse::Core::SQL::NonBlockingPrepareStatement::doExecute: Can only use this prepare inside a NisseHandler");
    }
    auto& service = Service::Server::getCurrentHandler();
    service.transferHandler<MySQLExecuteHandler>(connection, *this);
}
