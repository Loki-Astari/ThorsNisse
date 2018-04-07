---
layout: class
generate: false
typeInfo:
    namespace: ThorsAnvil::Nisse::Core::Socket
    header:    ThorsNisseCoreSocket/Socket.h
    classname: ServerSocket
    parent:    BaseSocket
    description: 
    methods:
        - return:
          name: Constructor
          param: ['int', 'bool', 'int']
          mark:
        - return: DataSocket
          name: accept
          param: [bool]
          mark:
children:
    - name: serversocket::accept
      value: core.socket.ServerSocket.accept.md
---

