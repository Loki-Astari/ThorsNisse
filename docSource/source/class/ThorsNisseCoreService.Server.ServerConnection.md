---
layout: class
generate: false
typeInfo:
    namespace: ThorsAnvil::Nisse::Core::Service
    header:    ThorsNisseCoreService/Server.h
    classname: ServerConnection
    parent:    
    description:  |
        Used to simplify the definition of a port.
    methods:
        - return: ' '
          name: Constructor
          param: [  ' int port', ' int maxConnections = ThorsAnvil::Nisse::Core::Socket::ServerSocket::maxConnectionBacklog) : port(port) ', ' maxConnections(maxConnections) {} }; // @class // An object that acts as the main server event loop. // One of these objects can handle all the ports your application requires class Server { private: bool running; bool shutDownNext; EventHolder eventBase; // The handlers are held by pointer because they are // polymorphic (there can potentially be a lot of different // handlers for different incoming streams' ]
          mark:  .
    protected: []
    virtual: []
children:
    - name: Constructor
      value: ThorsNisseCoreService.ServerConnection.methods.1.md
---
```cpp
    Serv::Server      server;
    // Implicit creation of ServerConnection
    server.listenOn<MyHandler>(8080);
    // Explicit creation of ServerConnection
    server.listenOn<MyHandler>(Serv::ServerConnection(8081, 20));
```

