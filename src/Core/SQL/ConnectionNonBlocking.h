#ifndef THORSANVIL_NISSE_CORE_SQL_CONNECTION_NON_BLOCKING_H
#define THORSANVIL_NISSE_CORE_SQL_CONNECTION_NON_BLOCKING_H

#include "ThorsMySQL/Connection.h"
#include "ThorsMySQL/MySQLStream.h"

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace Core
        {
            namespace SQL
            {

namespace DBMySQL = ThorsAnvil::DB::MySQL;
namespace DB =      ThorsAnvil::DB::Access;
class MySQLConnectionHandler;

class ConnectionNonBlocking: public DBMySQL::Connection
{
    public:
        ConnectionNonBlocking(DBMySQL::MySQLStream& stream,
                          std::string const& username,
                          std::string const& password,
                          std::string const& database,
                          DB::Options const& options,
                          DBMySQL::ConectReader& packageReader,
                          DBMySQL::ConectWriter& packageWriter);

    private:
        friend class MySQLConnectionHandler;
        void doConectToServer(std::string const& username,
                              std::string const& password,
                              std::string const& database,
                              DB::Options const& options
                             );
};

            }
        }
    }
}

#endif
