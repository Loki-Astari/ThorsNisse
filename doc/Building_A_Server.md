[![Build Status](https://travis-ci.org/Loki-Astari/ThorsNisse.svg?branch=master)](https://travis-ci.org/Loki-Astari/ThorsNisse)

![ThorStream](../img/Nisse.jpg)

## Server
There is no actual need to build a server manually. A Server that supports the HTTP protocol is provided with the package. This documentation is provided if you need to build additions to the framework to support other protocols.

A very simple server can be written like this:


### Example Server:
Compile With:
````
g++ -std=c++1z server.cpp -lThorsExpress17
````

````C++
#include "ThorsNisseCoreService/Server.h"
#include "ThorsNisseCoreService/Handler.h"

int main()
{
    try
    {
        namespace Node      = ThorsAnvil::Nisse::Core::Service;

        Node::Server        service;
        YourData            toEachRequest;

        // You specify the class that will handle a request on a specific port here with listOn()
        // method. `YourHandlerClass` must be derived from Serice::HandlerBase. On a connection
        // an instance of the class is created and passed `toEachRequest` as a parameter to the
        // constructor. Once constructed the method `eventActivate()` will be called when the
        // connected port is usable.
        service.listenOn<YourHandlerClass>(40716, toEachRequest);
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

### Next Steps
If you want to build your own protocol handler then read up on [Handler](interface/handler.md.md).

If you want to to use the HTTP protocol handler then read about [Modules](Building_A_Module.md).


