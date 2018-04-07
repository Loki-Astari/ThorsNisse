---
layout: class
generate: false
typeInfo:
    namespace: ThorsAnvil::Nisse::Core::Socket
    header:    ThorsNisseCoreSocket/Socket.h
    classname: DataSocket
    parent:    BaseSocket
    description: Data sockets define the read/write interface to a socket.<br>This class should not be directly created
    methods:
        - return: std::pair<bool, std::size_t>
          name: getMessageData
          param: ['char*', 'std::size_t', 'std::size_t']
          mark:
        - return: std::pair<bool, std::size_t>
          name: putMessageData
          param: ['char*', 'std::size_t size', 'std::size_t size']
          mark:
        - return: void
          name: putMessageClose
          param: []
          mark:
children:
    - name: DataSocket::getMessageData
      value: core.socket.DataSocket.getMessageData.md
    - name: DataSocket::putMessageData
      value: core.socket.DataSocket.putMessageData.md
    - name: DataSocket::putMessageClose
      value: core.socket.DataSocket.putMessageClose.md

---

