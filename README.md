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

This is RESTful server framework for C++.  
It is base on the Node.js server and uses transparently non-blocking streams to achieve high throughput and low complexity of development on a single threaded server.

## Goal

The goal is to build a framework that allows functors (including lambda's) to be used to specify the actions of a RESTful end point. When invoked these functors will behave like normal C++ routines that use blocking streams (thus allowing for very standard C++ coding style); but the streams themselves will behave like non-blocking streams and on a blocking request transfer control back to Nisse server framework.

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

## Naming

