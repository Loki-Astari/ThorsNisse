[![Build Status](https://travis-ci.org/Loki-Astari/ThorsNisse.svg?branch=master)](https://travis-ci.org/Loki-Astari/ThorsNisse)

![ThorStream](../img/Nisse.jpg)

### Example Server:
Compile With:

````
g++ -std=c++1z ser.cpp -lThorsExpress17 -lThorSQL17 -lThorMySQL17 -lThorSerialize17
````

````C++
#include "ThorsNisseCoreService/Server.h"
#include "ThorsNisseCoreService/Handler.h"
#include "ThorsNisseProtocolHTTP/HTTPProtocol.h"
#include "ThorSQL/Connection.h"
#include "ThorSQL/Statement.h"
#include "ThorSerialize/JsonThor.h"

struct BeerItem
{
    long        id;
    std::string name;
    int         price;
};

ThorsAnvil_MakeTrait(BeerItem, id, name, price);

int main()
{
    try
    {
        namespace Node      = ThorsAnvil::Nisse::Core::Service;
        namespace Express   = ThorsAnvil::Nisse::Protocol::HTTP;
        namespace ThorsSQL  = ThorsAnvil::SQL;
        namespace ThorsSer  = ThorsAnvil::Serialize;

        ThorsSQL::Connection    mysql("mysql:://host", "username", "password", {});
        ThorsSQL::Statement     beerList(mysql, "SELECT Name, Price FROM Beer WHERE Quality > ?");

        Express::Site          site;
        site.get("/listBeer.html",[&beerList](Express::Request& request, Express::Response& response)
        {
            response.headers["Type"] = "BeerList";

            // The interesting part here. Is that the << operator
            // will return to the main loop if it would block.
            // When the socket is ready to accept more data then
            // The lambda is restarted at the point it left off.
            // So we get a non blocking single threaded server.
            response.body << "<html><head></head><body><h1>We have beer</h1></body><ul>";

            // Now using the same transparently non-blocking technique with SQL query
            beerList.execute(ThorsSQL::Bind(15),
                // This method called as each row is decoded from the DB
                [&response](long id, std::string const& name, int price){
                    response.body << "<li>" << name << " : " << price << "</li>";
                }
            );
            response.body << "</ul></html>";
        });
        site.get("/listBeer.json",[&beerList](Express::Request& request, Express::Response& response)
        {
            response.headers["Type"]         = "BeerList";
            response.headers["Content-Type"] = "text/json";

            bool                    firstRow = true;

            response.body << R"({"BeerList": [)";

            beerList.execute(ThorsSQL::Bind(22),
                [&response, &firstRow](long id, std::string const& name, int price) {
                    BeerItem    item{id, std::move(name), price};
                    response.body << (firstRow ? "": ",") << ThorsSer::jsonExport(item);
                    firstRow = false;
                }
            );
            response.body << R"(], "RowCount": )" << beerList.rowsAffected() << "}";
        });

        Express::Binder      binder;
        binder.addSite("ThorsAnvil.com", "", std::move(site));

        Node::Server         service;
        service.listenOn<Express::ReadRequestHandler>(40716, binder);
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
````

