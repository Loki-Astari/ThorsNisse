---
layout: class
generate: false
typeInfo:
    namespace: ThorsAnvil::Nisse::Core::Socket
    header:    ThorsNisseCoreSocket/Socket.h
    classname: ServerSocket
    parent:    BaseSocket
    description:  |
        A server socket that listens on a port for a connection
    methods:
        - return: ' '
          name: Constructor
          param: [  ' int port', ' bool blocking = false', ' int maxWaitingConnections = maxConnectionBacklog' ]
          mark:  
        - return: ' DataSocket'
          name: accept
          param: [  ' bool blocking = false' ]
          mark:  
    protected: []
    virtual: []
children:
    - name: Constructor
      value: ThorsNisseCoreSocket.Socket.ServerSocket.methods.1.Constructor.md
    - name: accept
      value: ThorsNisseCoreSocket.Socket.ServerSocket.methods.2.accept.md
---
