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
          param: [  ' int socketId', ' bool blocking = false) : BaseSocket(socketId', ' blocking) {} // @method // Reads data from a sokcet into a buffer. // If the stream is blocking will not return until the requested amount of data has been read or there is no more data to read. // If the stream in non blocking will return if the read operation would block. // @return This method returns a <code>std::pair&lt;bool', ' std::size_t&gt;</code>. The first member `bool` indicates if more data can potentially be read from the stream. If the socket was cut or the EOF reached then this value will be false. The second member `std::size_t` indicates exactly how many bytes were read from this stream. // @param buffer The buffer data will be read into. // @param size The size of the buffer. // @param alreadyGot Offset into buffer (and amount size is reduced by' ]
          mark:  as this amount was read on a previous call).
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
      value: ThorsNisseCoreSocket.DataSocket.methods.1.md
    - name: getMessageData
      value: ThorsNisseCoreSocket.DataSocket.methods.2.md
    - name: putMessageData
      value: ThorsNisseCoreSocket.DataSocket.methods.3.md
    - name: putMessageClose
      value: ThorsNisseCoreSocket.DataSocket.methods.4.md
---
