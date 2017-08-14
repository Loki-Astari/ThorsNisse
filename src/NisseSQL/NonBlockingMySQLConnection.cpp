#include "NonBlockingMySQLConnection.h"
#include "NonBlockingPrepareStatement.h"

using namespace ThorsAnvil::NisseSQL;

NonBlockingMySQLConnection::NonBlockingMySQLConnection(
                            std::string const& host, int port,
                            std::string const& username,
                            std::string const& password,
                            std::string const& database,
                            ThorsAnvil::SQL::Options const& options)
    : stream(host, port, true)
    , buffer(stream, true)
    , reader(buffer)
    , writer(buffer)
    , connection(stream, username, password, database, options, reader, writer)
{}

std::unique_ptr<ThorsAnvil::SQL::Lib::StatementProxy>
NonBlockingMySQLConnection::createStatementProxy(std::string const& statement)
{
    std::unique_ptr<ThorsAnvil::SQL::Lib::StatementProxy>  result;
    result.reset(new NonBlockingPrepareStatement(connection, statement));
    return result;
}

ThorsAnvil::SQL::Lib::ConnectionCreatorRegister<NonBlockingMySQLConnection>    mysqlConnection("mysqlNB");
