---
layout: class
generate: false
typeInfo:
    namespace: ThorsAnvil::Nisse::Core::Socket
    header:    ThorsNisseCoreSocket/Socket.h
    classname: DataSocket
    parent:    BaseSocket
    description:  |
        Data sockets define the read/write interface to a socket.
        This class should not be directly created
    methods:
        - return: ' '
          name: Constructor
          param: [  ' int socketId', ' bool blocking = false' ]
          mark:  
        - return: ' std::pair<bool, std::size_t>'
          name: getMessageData
          param: [  ' char* buffer', ' std::size_t size', ' std::size_t alreadyGot = 0' ]
          mark:  
        - return: ' std::pair<bool, std::size_t>'
          name: putMessageData
          param: [  ' char const* buffer', ' std::size_t size', ' std::size_t alreadyPut = 0' ]
          mark:  
        - return: ' void '
          name: putMessageClose
          param: [  ]
          mark: 
    protected: []
    virtual: []
children:
    - name: Constructor
      value: ThorsNisseCoreSocket.Socket.DataSocket.methods.1.Constructor.md
    - name: getMessageData
      value: ThorsNisseCoreSocket.Socket.DataSocket.methods.2.getMessageData.md
    - name: putMessageData
      value: ThorsNisseCoreSocket.Socket.DataSocket.methods.3.putMessageData.md
    - name: putMessageClose
      value: ThorsNisseCoreSocket.Socket.DataSocket.methods.4.putMessageClose.md
---
