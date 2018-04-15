---
layout: method
generate: false
methodInfo:
    parent: Server
    name: listenOn
    mark:  
    description:  |
        This is a templatized function. The template type is the type of object that will handle a connection once it has been established.
        When a connection is accepted and object of the template type is created and passed a reference to the server, stream and `data` (see parameters).
        The object is marked as listening to the accepted socket and when data is available on the socket the <code>eventActivate()</code> method is called allowing the data to be processed.
        For more details see <a href="#HandlerBase">HandlerBase</a>.
    return:
        type: 'void'
        description: 
    parameters:
        - name: info
          type: 'ServerConnection const&'
          default: 
          description: 'The port that is being listen too.'
        - name: param
          type: 'Param&'
          default: 
          description: 'A reference to an object that is passed to the constructor of the handler type. This allows a state object to be passed to the constructor.'
---
```cpp
class MyState
{
    // Stuf
};
class MyHandler: public Serv::HandlerSuspendableWithStream
{
    public:
        // Notice the last parameter is `state`.
        // This means a state object must be provided to the `listenOn()` method.
        MyHandler(Serv::Server& parent, Sock::DataSocket&& stream, MyState& state)
            : HandlerSuspendableWithStream(parent, std::move(stream), EV_READ)
        {}

        virtual bool eventActivateWithStream(std::istream& input, std::ostream& output)
        { // Stuff }
};
int main()
{
    Serv::Server      server;
    MyStateObject     state;

    // Because we pass state to the `listenOn()` method; this will be passed to the constructor of 
    // `MyHandler` when it is created. This allows a user defined state object to be used consistently
    // across all connections.
    server.listenOn<MyHandler>(8080, state);

    // Start the event loop.
    server.start();
}
```

