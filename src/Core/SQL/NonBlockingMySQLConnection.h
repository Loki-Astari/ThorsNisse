#ifndef THORS_ANVIL_NISSESQL_NONBLOCKING_MY_SQL_CONNECTION_H
#define THORS_ANVIL_NISSESQL_NONBLOCKING_MY_SQL_CONNECTION_H

#include "ConnectionNonBlocking.h"
#include "ThorMySQL/MySQLStream.h"
#include "ThorMySQL/PackageBuffer.h"
#include "ThorMySQL/ConectReader.h"
#include "ThorMySQL/ConectWriter.h"
#include "ThorSQL/Connection.h"
#include <string>
#include <memory>
#include <functional>

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace Core
        {
            namespace SQL
            {

namespace MySQL = ThorsAnvil::MySQL;

class NonBlockingMySQLConnection: public ThorsAnvil::SQL::Lib::ConnectionProxy
{
    private:
        MySQL::MySQLStream                        stream;
        MySQL::PackageBuffer<MySQL::MySQLStream>  buffer;
        MySQL::ConectReader                       reader;
        MySQL::ConectWriter                       writer;
        ConnectionNonBlocking                     connection;
    public:
        NonBlockingMySQLConnection(std::string const& host, int port,
                               std::string const& username,
                               std::string const& password,
                               std::string const& database,
                               ThorsAnvil::SQL::Options const& options);
        virtual std::unique_ptr<ThorsAnvil::SQL::Lib::StatementProxy> createStatementProxy(std::string const& statement) override;
        virtual int getSocketId() const override;
        virtual void setYield(std::function<void()>&&, std::function<void()>&&) override;
};

            }
        }
    }
}

#endif
