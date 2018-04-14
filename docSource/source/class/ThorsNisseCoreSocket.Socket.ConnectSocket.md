---
layout: class
generate: false
typeInfo:
    namespace: ThorsAnvil::Nisse::Core::Socket
    header:    ThorsNisseCoreSocket/Socket.h
    classname: ConnectSocket
    parent:    DataSocket
    description:  |
        Creates a connection to <code>host</code> on <code>port</code>.
        Note this class inherits from <code>DataSocket</code> so once created you can read/write to the socket.
    methods:
        - return: ' '
          name: Constructor
          param: [  ' std::string const& host', ' int port' ]
          mark:  
    protected: []
    virtual: []
children:
    - name: Constructor
      value: ThorsNisseCoreSocket.ConnectSocket.methods.1.md
---
