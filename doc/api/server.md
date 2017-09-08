[![Build Status](https://travis-ci.org/Loki-Astari/ThorsNisse.svg?branch=master)](https://travis-ci.org/Loki-Astari/ThorsNisse)

![ThorStream](../../img/Nisse.jpg)

### Creating a Server Object

* Server();  
Servers take no parameters. It is simply the server.

* Server(Server&&);
* Server& operator=(Server&&);  
The server can be moved (but not copied).

### Starting a Server Loop

* void start(double check = 10.0)  
The method `start()` initiates the main even loop of the server. This function will not return until `flagShutDown()` has been called.  
The `check` parameter is how often (in seconds) the main loop will perform house keeping tasks. For simple servers a 10s value seems to work. For high volume servers this may need to be lowered.

* void flagShutDown();  
Initiates a server shutdown. The main event loop of the server will exit at the next `check` point.

### Adding Handlers

* template<typename Handler, typename Param> void listenOn(ServerConnection const& info, Param& param);  
The `listenOn()` method adds a handler type `Handler` to a port.  
The `Handler` type must be derived from `Service::HandlerBase`. See [Handler](handler.md) documentation.  
When a connection is made to port; upon accepting the connection the server will create an object of type `Handler` and set up appropriate listeners for the port. When the listeners indicate that data is available on the port the `eventActivate()` method is called to handler the data.

* void addTimer(double timeOut, std::function<void()>&& action);  
The `addTimer()` mthod adds a time based handler. Thus allowing your code to speciy periodic action the server needs to perform.

