#include "ThorsNisse/NisseService.h"
#include "ThorsNisse/NisseHandler.h"
#include "ThorsImp/ProtocolSimple.h"
#include "ThorsImp/ProtocolSimpleStream.h"
#include "ThorsTroll/HTTPProtocol.h"
#include "ThorsTroll/Binder.h"
#include "ThorsTroll/Types.h"
#include "ThorSQL/Connection.h"
#include "ThorSQL/Statement.h"

#include <iostream>

namespace Nisse = ThorsAnvil::Nisse;
namespace HTTP  = ThorsAnvil::Nisse::ProtocolHTTP;
namespace SQL   = ThorsAnvil::SQL;

void listBeer(HTTP::Request& /*request*/, HTTP::Response& response)
{
    response.body << "<html>"
                  << "<head><title>Beer List</title></head>"
                  << "<body>"
                  << "<h1>Beer List</h1>"
                  << "<ol>";

    SQL::Connection connection("mysqlNB://test.com", "test", "testPassword", "test");
    SQL::Statement  listBeers(connection, "SELECT Name, Age FROM Beers");

    listBeers.execute([&response](std::string const& name, int age)
        {
            response.body << "<li>" << name << " : " << age << "</li>";
        }
    );


    response.body << "</ol></body></html>";
}

int main()
{
    try
    {
        HTTP::Site      site;
        site.get("/listBeer",listBeer);
        site.post("/addBeer", [](HTTP::Request& /*request*/, HTTP::Response& /*response*/){std::cerr << "AddBeer\n";});

        HTTP::Binder    binder;
        binder.addSite("test.com", std::move(site));

        Nisse::NisseService    service;
        service.listenOn<HTTP::ReadRequestHandler>(40716, binder);

        /*
        using ThorsAnvil::Nisse::ProtocolSimple::ReadMessageHandler;
        service.listenOn<ReadMessageHandler>(40717);

        using ThorsAnvil::Nisse::ProtocolSimple::ReadMessageStreamHandler;
        service.listenOn<ReadMessageStreamHandler>(40718);
        */

        service.start();
    }
    catch (std::exception const& e)
    {
        //Log exception
        throw;
    }
    catch (...)
    {
        //Log exception
        throw;
    }
}
