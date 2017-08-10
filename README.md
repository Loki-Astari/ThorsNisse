[![Build Status](https://travis-ci.org/Loki-Astari/ThorsNisse.svg?branch=master)](https://travis-ci.org/Loki-Astari/ThorsNisse)
[![Code Review](http://www.zomis.net/codereview/shield/?qid=169216)](http://codereview.stackexchange.com/q/169216/507)
[![Code Review](http://www.zomis.net/codereview/shield/?qid=168793)](http://codereview.stackexchange.com/q/168793/507)
[![Code Review](http://www.zomis.net/codereview/shield/?qid=168792)](http://codereview.stackexchange.com/q/168792/507)
[![Code Review](http://www.zomis.net/codereview/shield/?qid=168790)](http://codereview.stackexchange.com/q/168790/507)
[![Code Review](http://www.zomis.net/codereview/shield/?qid=168772)](http://codereview.stackexchange.com/q/168772/507)
[![Code Review](http://www.zomis.net/codereview/shield/?qid=168771)](http://codereview.stackexchange.com/q/168771/507)
[![Code Review](http://www.zomis.net/codereview/shield/?qid=168767)](http://codereview.stackexchange.com/q/168767/507)

![ThorStream](img/Nisse.jpg)

# ThorsNisse

This is a simple to use WebServer based on the design principles of Node.js.

## Goal

The goal is to build a framework that allows lambda's to be used to specify the actions when specific end points are hit. The lambads will be provided with stream like interfaces (so coding is still very much C++ like but the streams themselves will be transparently non-blocking. This means to the lambda the streams act like blocking streams but behind the scenes a blocking read/write will result in control being returned to the server to continue processing the next request.

A secondary goal is to use my Serializing library ([ThorsSerializer](https://github.com/Loki-Astari/ThorsSerializer)) and incorporate the SQL Library ([ThorsSQL](https://github.com/Loki-Astari/ThorsSQL)).

### Example Usage:

````C++
int main()
{
    try
    {
        namespace Node      = ThorsAnvil::Nisse;
        namespace Express   = ThorsAnvil::Nisse::ProtocolHTTP;
        namespace ThorsSQL  = ThorsAnvil::SQL;
        namespace ThorsSer  = ThorsAnvil::Serialize;

        Express::Site          site;
        site.add("/listBeer.html",[](Express::Request& request, Express::Response& response)
        {
            response.headers["Type"] = "BeerList";

            // The interesting part here. Is that the << operator
            // will return to the main loop if it would block.
            // When the socket is ready to accept more data then
            // The lambda is restarted at the point it left off.
            // So we get a non blocking single threaded server.
            response.body << "<html><head></head><body><h1>We have beer</h1></body><ul>";

            // Now using the same transparently non-blocking technique with SQL query
            ThorsSQL::Connection    mysql("mysql:://host", "username", "password");
            ThorsSQL::Statement     beerList(mysql, "SELECT Name, Price FROM Beer WHERE Quality > ?");

            beerList.execute(ThorsSQL::Bind(request.uri.query("quality")),
                // This method called as each row is decoded from the DB
                [&response](u64 id, std::string const& name, int price){
                    response.body << "<li>" << name << " : " << price << "</li>";
                },
                // This method is called when all the rows have been recieved.
                [&response](long /*rowCount*/){
                    response.body << "</ul></html>";
                }
            );
        }
        site.add("/listBeer.json",[](Express::Request& request, Express::Response& response)
        {
            response.headers["Type"]         = "BeerList";
            response.headers["Content-Type"] = "text/json";

            ThorsSQL::Connection    mysql("mysql:://host", "username", "password");
            ThorsSQL::Statement     beerList(mysql, "SELECT Name, Price FROM Beer WHERE Quality > ?");
            bool                    firstRow = true;

            response.body << R"({"BeerList": [)";

            beerList.execute(ThorsSQL::Bind(request.uri.query("quality")),
                [&response, &firstRow](BeerItem const& item) {
                    response.body << (firstRow ? "", ",") << ThorsSer::jsonExport(item);
                    firstRow = false;
                },
                [&response](long rowCount){
                    response.body << R"(], "RowCount": )" << rowCount << "}";
                }
            );
        }

        Express::Binder      binder;
        binder.addSite("ThorsAnvil.com", std::move(site));

        Node::NisseService    service;
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

