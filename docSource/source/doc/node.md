---
layout: plain
generate: false
---

To simplify development for those that simply want to write HTTP RESTful endpoints the server Node.Cxx is provided to handle all the boilerplate.

Node.Cxx (Lovingly named after Node.js for the inspiration) is a simple HTTP server implemented using the ThorsNisse framework that will dynamically load [Express](#express) modules.

The Node.Cxx server takes two flags as arguments on startup:

```bash
 > Node.cxx [--debug:<port>] [--load:<SharedLib>:<Host>:<Base>:<port>]
 #
 # --debug:8081
 #      Loads the dynamic loader so it listens on port 8081
 #          To load a libray:
 #              curl --data '{"action": "Load",   "host": "test.com", "base": "", "port": 8080, "lib": "AddBeer.dylib"}' localhost:8081
 #          To unload a library:
 #              curl --data '{"action": "Unload", "host": "test.com", "base": "", "port": 8080}'  localhost:8081
 #
 #
 # --load:AddBeer.dylib:test.com::8080
 #      Loads the AddBeer.dylib express module.
 #          And binds it to the root of http://test.com
 #
 #          Assume AddBeer defines the end pointes /AddBeer
 #          When a request to `http://test.com/AddBeer the associated code will be called.
 #
 # --load:AddDrink.dylib:test.com:/drinks:8080
 #      Loads the AddDrink.dylib express module.
 #          And binds it to http://test.com/drinks
 #
 #          Assume AddDrink defines the end pointes /AddDrink
 #          When a request to `http://test.com/drinks/AddDrink the associated code will be called.
 #
 #          Note we did not bind AddDrink to the root of test.com so you need to add the `Base` to
 #          all routes defined by the library in its code.
```


|Command Line Flag        | Description
|------------|------------
|--debug     | Installs the dynamic library loader.<br>This allows shared libraries to be loaded/unloaded at runtime without restarting the server. Useful for building and debugging but should not be used in production.<br><br>As the command to load/unload library is simply a REST call to the specified port; this can be included into your build environment to un-install the old version and install the newly build version allowing a quick turn around for development.
|--load      | Loads a shared library that is linked with [Express](#express).<br>This flag can be used multiple times.<br><br>The express module provides the facility to easily associate code with REST end points.<br><br>This flag uses the same underlying code as the REST call to load a site. So these sites can also be dynamically unloaded if required.

