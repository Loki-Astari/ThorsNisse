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
          param: [  ' int port', ' int maxConnections = ThorsAnvil::Nisse::Core::Socket::ServerSocket::maxConnectionBacklog' ]
          mark:  
    protected: []
    virtual: []
children:
    - name: Constructor
      value: ThorsNisseCoreService.Server.ServerConnection.methods.1.Constructor.md
---
```cpp
    Serv::Server      server;
    // Implicit creation of ServerConnection
    server.listenOn<MyHandler>(8080);
    // Explicit creation of ServerConnection
    server.listenOn<MyHandler>(Serv::ServerConnection(8081, 20));
```

