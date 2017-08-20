#ifndef THORSANVIL_NISSE_CORE_SQL_NONBLOCKING_PREPARE_STATEMENT_H
#define THORSANVIL_NISSE_CORE_SQL_NONBLOCKING_PREPARE_STATEMENT_H

#include "ConnectionNonBlocking.h"
#include "ThorSQL/Statement.h"

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace Core
        {
            namespace SQL
            {

using StatmentPIMPL = std::unique_ptr<ThorsAnvil::SQL::Lib::StatementProxy>;
class NonBlockingMySQLConnection;
class NonBlockingPrepareStatement: public ThorsAnvil::SQL::Lib::StatementProxy
{
    StatmentPIMPL               prepareStatement;
    NonBlockingMySQLConnection& connection;
    public:
        NonBlockingPrepareStatement(NonBlockingMySQLConnection& connection, ConnectionNonBlocking& nbStream, std::string const& statement);
        void createProxy(ConnectionNonBlocking& nbStream, std::string const& statement);

            virtual void doExecute()                            override;
            void executePrepare()                               {prepareStatement->doExecute();}


            // Utility Methods
            virtual void   abort()                              override {prepareStatement->abort();}
            virtual bool   more()                               override {return prepareStatement->more();}
            virtual bool   isSelect() const                     override {return prepareStatement->isSelect();}
            virtual long   rowsAffected() const                 override {return prepareStatement->rowsAffected();}
            virtual long   lastInsertID() const                 override {return prepareStatement->lastInsertID();}

            // bind
            virtual void   bind(char v)                         override {prepareStatement->bind(v);}
            virtual void   bind(signed char v)                  override {prepareStatement->bind(v);}
            virtual void   bind(signed short v)                 override {prepareStatement->bind(v);}
            virtual void   bind(signed int v)                   override {prepareStatement->bind(v);}
            virtual void   bind(signed long v)                  override {prepareStatement->bind(v);}
            virtual void   bind(signed long long v)             override {prepareStatement->bind(v);}
            virtual void   bind(unsigned char v)                override {prepareStatement->bind(v);}
            virtual void   bind(unsigned short v)               override {prepareStatement->bind(v);}
            virtual void   bind(unsigned int v)                 override {prepareStatement->bind(v);}
            virtual void   bind(unsigned long v)                override {prepareStatement->bind(v);}
            virtual void   bind(unsigned long long v)           override {prepareStatement->bind(v);}

            virtual void   bind(float v)                        override {prepareStatement->bind(v);}
            virtual void   bind(double v)                       override {prepareStatement->bind(v);}
            virtual void   bind(long double v)                  override {prepareStatement->bind(v);}

            virtual void   bind(std::string const& v)           override {prepareStatement->bind(v);}
            virtual void   bind(std::vector<char> const& v)     override {prepareStatement->bind(v);}

            virtual void   bind(ThorsAnvil::SQL::UnixTimeStamp const& v)    override {prepareStatement->bind(v);}

            // retrieve
            virtual void   retrieve(char& v)                    override {prepareStatement->retrieve(v);}
            virtual void   retrieve(signed char& v)             override {prepareStatement->retrieve(v);}
            virtual void   retrieve(signed short& v)            override {prepareStatement->retrieve(v);}
            virtual void   retrieve(signed int& v)              override {prepareStatement->retrieve(v);}
            virtual void   retrieve(signed long& v)             override {prepareStatement->retrieve(v);}
            virtual void   retrieve(signed long long& v)        override {prepareStatement->retrieve(v);}
            virtual void   retrieve(unsigned char& v)           override {prepareStatement->retrieve(v);}
            virtual void   retrieve(unsigned short& v)          override {prepareStatement->retrieve(v);}
            virtual void   retrieve(unsigned int& v)            override {prepareStatement->retrieve(v);}
            virtual void   retrieve(unsigned long& v)           override {prepareStatement->retrieve(v);}
            virtual void   retrieve(unsigned long long& v)      override {prepareStatement->retrieve(v);}

            virtual void   retrieve(float& v)                   override {prepareStatement->retrieve(v);}
            virtual void   retrieve(double& v)                  override {prepareStatement->retrieve(v);}
            virtual void   retrieve(long double& v)             override {prepareStatement->retrieve(v);}

            virtual void   retrieve(std::string& v)             override {prepareStatement->retrieve(v);}
            virtual void   retrieve(std::vector<char>& v)       override {prepareStatement->retrieve(v);}

            virtual void   retrieve(ThorsAnvil::SQL::UnixTimeStamp& v)      override {prepareStatement->retrieve(v);}
};

            }
        }
    }
}

#endif
