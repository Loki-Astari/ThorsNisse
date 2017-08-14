#include "Socket/Utility.h"
#include "NonBlockingMySQLConnection.h"
#include "NonBlockingPrepareStatement.h"
#include "ThorsNisse/NisseService.h"
#include "ThorsNisse/NisseHandler.h"
#include "ThorsNisse/CoRoutine.h"
#include "ThorMySQL/PrepareStatement.h"

using namespace ThorsAnvil::NisseSQL;

using CoRoutine = ThorsAnvil::Nisse::CoRoutine::Context<short>::pull_type;
using Yield     = ThorsAnvil::Nisse::CoRoutine::Context<short>::push_type;

class MySQLPrepareHandler: public ThorsAnvil::Nisse::NisseHandler
{
    CoRoutine   worker;
    public:
        MySQLPrepareHandler(ThorsAnvil::Nisse::NisseService& service,
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

        virtual short eventActivate(ThorsAnvil::Nisse::LibSocketId /*sockId*/, short /*eventType*/) override
        {
            if (!worker())
            {
                dropHandler();
                return 0;
            }
            return worker.get();
        }
};

class MySQLExecuteHandler: public ThorsAnvil::Nisse::NisseHandler
{
    CoRoutine   worker;
    public:
        MySQLExecuteHandler(ThorsAnvil::Nisse::NisseService& service,
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

        virtual short eventActivate(ThorsAnvil::Nisse::LibSocketId /*sockId*/, short /*eventType*/) override
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
    if (!ThorsAnvil::Nisse::NisseService::inHandler())
    {
        throw std::runtime_error("ThorsAnvil::NisseSQL::NonBlockingPrepareStatement::NonBlockingPrepareStatement: Can only use this prepare inside a NisseHandler");
    }
    auto& service = ThorsAnvil::Nisse::NisseService::getCurrentHandler();
    service.transferHandler<MySQLPrepareHandler>(connection, *this, nbStream, statement);
}

void NonBlockingPrepareStatement::doExecute()
{
    if (!ThorsAnvil::Nisse::NisseService::inHandler())
    {
        throw std::runtime_error("ThorsAnvil::NisseSQL::NonBlockingPrepareStatement::doExecute: Can only use this prepare inside a NisseHandler");
    }
    auto& service = ThorsAnvil::Nisse::NisseService::getCurrentHandler();
    service.transferHandler<MySQLExecuteHandler>(connection, *this);
}
