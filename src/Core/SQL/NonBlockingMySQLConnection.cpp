#include "NonBlockingMySQLConnection.h"
#include "NonBlockingPrepareStatement.h"

using namespace ThorsAnvil::Nisse::Core::SQL;

NonBlockingMySQLConnection::NonBlockingMySQLConnection(
                            std::string const& host, int port,
                            std::string const& username,
                            std::string const& password,
                            std::string const& database,
                            DB::Options const& options)
    : stream(host, port)
    , buffer(stream, true)
    , reader(buffer)
    , writer(buffer)
    , connection(stream, username, password, database, options, reader, writer)
{}

std::unique_ptr<ThorsAnvil::DB::Access::Lib::StatementProxy>
NonBlockingMySQLConnection::createStatementProxy(std::string const& statement)
{
    std::unique_ptr<ThorsAnvil::DB::Access::Lib::StatementProxy>  result;
    result.reset(new NonBlockingPrepareStatement(*this, connection, statement));
    return result;
}

int NonBlockingMySQLConnection::getSocketId() const
{
    return stream.getSocketId();
}

void NonBlockingMySQLConnection::setYield(std::function<void()>&& yr, std::function<void()>&& yw)
{
    stream.setYield(std::move(yr), std::move(yw));
}

void NonBlockingMySQLConnection::close()
{}

ThorsAnvil::DB::Access::Lib::ConnectionCreatorRegister<NonBlockingMySQLConnection>    mysqlNBConnection("mysqlNB");
