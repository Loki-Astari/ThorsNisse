#ifndef THORSANVIL_NISSE_CORE_SQL_NONBLOCKING_MYSQL_CONNECTION_H
#define THORSANVIL_NISSE_CORE_SQL_NONBLOCKING_MYSQL_CONNECTION_H

#include "ConnectionNonBlocking.h"
#include "ThorsMySQL/MySQLStream.h"
#include "ThorsMySQL/PackageBuffer.h"
#include "ThorsMySQL/ConectReader.h"
#include "ThorsMySQL/ConectWriter.h"
#include "ThorsDB/Connection.h"
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

namespace DBMySQL = ThorsAnvil::DB::MySQL;

class NonBlockingMySQLConnection: public ThorsAnvil::DB::Access::Lib::ConnectionProxy
{
    private:
        DBMySQL::MySQLStream                        stream;
        DBMySQL::PackageBuffer                      buffer;
        DBMySQL::ConectReader                       reader;
        DBMySQL::ConectWriter                       writer;
        ConnectionNonBlocking                     connection;
    public:
        NonBlockingMySQLConnection(std::string const& host, int port,
                               std::string const& username,
                               std::string const& password,
                               std::string const& database,
                               ThorsAnvil::DB::Access::Options const& options);
        virtual std::unique_ptr<ThorsAnvil::DB::Access::Lib::StatementProxy> createStatementProxy(std::string const& statement) override;
        virtual int getSocketId() const override;
        virtual void setYield(std::function<void()>&&, std::function<void()>&&) override;
        void close();
};

            }
        }
    }
}

#endif
