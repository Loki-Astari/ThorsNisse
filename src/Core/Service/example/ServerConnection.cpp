```cpp
    Serv::Server      server;
    // Implicit creation of ServerConnection
    server.listenOn<MyHandler>(8080);
    // Explicit creation of ServerConnection
    server.listenOn<MyHandler>(Serv::ServerConnection(8081, 20));
```
