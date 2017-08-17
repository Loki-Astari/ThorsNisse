#ifndef THORSANVIL_NISSE_CORE_SQL_CONNECTION_NON_BLOCKING_H
#define THORSANVIL_NISSE_CORE_SQL_CONNECTION_NON_BLOCKING_H

#include "ThorMySQL/Connection.h"
#include "ThorMySQL/MySQLStream.h"
#include "ThorsNisseCoreService/CoRoutine.h"

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace Core
        {
            namespace SQL
            {

using CoRoutine = Service::Context<short>::pull_type;
using Yield     = Service::Context<short>::push_type;

class MySQLConnectionHandler;
class ConnectionNonBlocking: public ThorsAnvil::MySQL::Connection
{
    public:
        ConnectionNonBlocking(ThorsAnvil::MySQL::MySQLStream& stream,
                          std::string const& username,
                          std::string const& password,
                          std::string const& database,
                          ThorsAnvil::SQL::Options const& options,
                          ThorsAnvil::MySQL::ConectReader& packageReader,
                          ThorsAnvil::MySQL::ConectWriter& packageWriter);

    private:
        friend class MySQLConnectionHandler;
        void doConectToServer(std::string const& username,
                              std::string const& password,
                              std::string const& database,
                              ThorsAnvil::SQL::Options const& options
                             );
};

            }
        }
    }
}

#endif
