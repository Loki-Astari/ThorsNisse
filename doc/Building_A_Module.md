[![Build Status](https://travis-ci.org/Loki-Astari/ThorsNisse.svg?branch=master)](https://travis-ci.org/Loki-Astari/ThorsNisse)

![ThorStream](../img/Nisse.jpg)

## Express
The Node.Cxx server and `Express` library provides two main features:

* Provides a handler for the HTTP protocol.  
This handler takes care of parsing incoming parameters (query/header) and automatically provides standard headers to the response.
* Provides a dynamic module loader.  
Modules that define a web site can be loaded and unloaded dynamically at runtime without stopping the server.

### HTTP Protocol Support

#### Example of Site
Compile With:
````
g++ -std=c++1z server.cpp -lThorsExpress17
````

````C++
#include "ThorsNisseExpress/all.h"

// SomeFunction() is provided solely so this is compilable code.
void SomeFunction()
{
    namespace Express   = ThorsAnvil::Nisse::Protocol::HTTP;
    Express::Site          site;

    site.get("/list.html",[&beerList](Express::Request& request, Express::Response& response)
    {
        // The `request` object contains all the information provided by the request stream.
        // All the standard parameters (query/header) have already been parsed for you.
        // If there is data in the body it is available via the std::istream object request.body

        // The `response` object is where you write output.
        // If you don't specify any headers or HTTP response then the following info will
        // automatically be returned => 200 OK  Headers => Date/Server/Connection/Content-Length
        // The content of the std::ostream response.body is used to calculate `Content-Length`
        // and automatically flushed.

        // The two stream objects `request.body` and `response.body` behave like normal blocking
        // stream to the function. This allows your code to be written like normal standard C++
        // that uses stream objects.

        // BUT: if a read/write to one of these stream objects would block then control is
        //      transferred back to the Nisse framework this allowing other request to be
        //      processed by the server.
        //
        //      Thus we provide pseudo paralleism with streams that behave as if they were
        //      blocking to the code using them but in reality are non blocking streams and
        //      thus will not cause your processes to halt.
    });
}
````

### Dynamic Loading

By default the Node.Cxx server does not allow modules to be loaded and unloaded dynamically (this is a security issue for a stable production server). But you can turn on this functionality when you start the server by specifying the `--debug:<port>` flag. Subsequent HTTP requests to the specific port allow modules to be loaded and unloaded dynamically.

#### Examples of how to load/unload modules
````
# To Load a module
curl --data '{"action": "Load", "lib": "<Shared Lib>", "host": "<Site Name>", "port": <number>, "base": "<Mount Point>"} 127.0.0.1:<port>/

# To Unload a module
curl --data '{"action": "Unoad", "host": "<Site Name>", "port": <number>, "base": "<Mount Point>"} 127.0.0.1:<port>/
````

#### Notes:
* Shared Lib:  
This is the either the full path to a shared library to load.  
Or this is the name of a shared library that is in the search path of `dlopen()`
* Site Name
This is the name of the site. More strictly this is the name in the `Host` header field.  
This value can be an empty string see [Binder](interface/Binder.md) for more information
* Mount Point
You can potentially have multiple site objects loaded onto the same "Site Name" just at different relative paths.
This value is usally empty to be a `Site` object to the root of the site.

#### How to write dynamically laadable module
Compile With:
````
g++ -std=c++1z server.cpp -lThorsExpress17
````
````C++
// Must include this header file.
#include "ThorsExpress/all.h"

namespace Express = ThorsAnvil::Nisse::Protocol::HTTP;

// Must define the function `addSite(Express::Site& site)`
// This function is called when the module is loaded to build the Site object.
void addSite(Express::Site& site)
{
    site.get("/list", [](Express::Request& request, Express::Response& response)
    {
        // Your code in here
    });
}
````





