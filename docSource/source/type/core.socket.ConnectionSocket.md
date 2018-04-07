---
layout: type
generate: false
typeInfo:
    namespace: ThorsAnvil::Nisse::Core::Socket
    header:    ThorsNisseCoreSocket/Socket.h
    classname: ConnectionSocket
    parent:    DataSocket
    description: Creates a connection to <code>host</code> on <code>port</code>.<br>Note this class inherits from <code>DataSocket</code> so once created you can read/write to the socket.
    methods:
        - return:
          name: Constructor
          param: ['std::string const&', 'int']
          mark:
---

