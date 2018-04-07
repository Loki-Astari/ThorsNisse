---
layout: package
generate: false
nameSpace:  ThorsAnvil::Nisse::Core::SQL
headers:
    base: ThorsNisseCoreSQL/
    files: []
---

```cpp
namespace Express = ThorsAnvil::Nisse::Protocol::HTTP;
namespace SQL     = ThorsAnvil::SQL;

static SQL::Connection connection("mysqlNB://test.com", "test", "testPassword", "test");
static SQL::Statement  listBeers(connection, "SELECT Name, Age FROM Beers");

class MyListBeerHandler: public HandlerSuspendable<DataSocket>
{
    virtual bool eventActivateNonBlocking()
    {
        ISocketStream   input(stream);

        std::string     request;
        std::getline(input, request);

        std::string     header;
        while(std::getline(input, header) && header != "\r")
        {}

        std::stringsttream body;
        body << "<html>"
             << "<head><title>Beer List</title></head>"
             << "<body>"
             << "<h1>Beer List</h1>"
             << "<ol>";

        // Calling execute() suspends the current handler.
        // Control is returned (unsuspends the handler) when the execute() function returns.
        //
        // The execute function returns when all the rows have been returned from the SQL
        // DB. The lambda is executes once for each row that is ruturned this allowing us
        // to build the response as a stream.
        listBeers.execute([&body](std::string const& name, int age)
        {
            // This call to the MySQL server is also non blocking.
            // If the call is going to block control is returned to ThorNisse framework
            // Allowing another connection to be processed while the MySQL server finishes
            // Its work.
            body << "<li>" << name << " : " << age << "</li>";
        });
        body << "</ol></body></html>";

        OSocketStream   output(stream);
        output << "HTTP/1.1 200 OK\r\n"
                  "Content-Length: " << body.size() << "\r\n"
                  "\r\n"
                  << body;
    }
};
```
```cpp--DeepDive
Please refer to the ThorSQL library for details.
```
A PIMPL proxy for the ThorSQL library.  
There are no user usable classes in this package. All the classes are used internally by ThorSQL library. There usage is unlocked by using the prefix "mysqlNB" in the connection string.

