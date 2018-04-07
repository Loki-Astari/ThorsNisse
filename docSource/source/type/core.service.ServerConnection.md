---
layout: type
generate: false
typeInfo:
    namespace: ThorsAnvil::Nisse::Core::Service
    header:    ThorsNisseCoreSocket/Server.h
    classname: ServerConnection
    parent:
    description:
    methods:
        - return:
          name: Constructor
          param: ['int', 'int']
          mark:
        - return: int
          name: port
          param:
          mark:
        - return: int
          name: maxConnections
          param:
          mark:
---
```cpp
    Serv::Server      server;
    // Implicit creation of ServerConnection
    server.listenOn<MyHandler>(8080);
    // Explicit creation of ServerConnection
    server.listenOn<MyHandler>(Serv::ServerConnection(8081, 20));
```

