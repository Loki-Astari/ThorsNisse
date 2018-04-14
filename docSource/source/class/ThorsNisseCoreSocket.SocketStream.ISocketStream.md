---
layout: class
generate: false
typeInfo:
    namespace: ThorsAnvil::Nisse::Core::Socket
    header:    ThorsNisseCoreSocket/SocketStream.h
    classname: ISocketStream
    parent:    std::istream
    description:  |
        An implementation of <code>std::istream</code> that uses <code>SocketStreamBuffer</code> as the buffer.
        The <code>Notofer</code> is a primitive event callback mechanism.
        A blocking read call to these streams calls the <code>Notifier noData</code>.
        This is used by the <a href="#Server">Server</a> infrastructure to yield control back to the main event loop.
        <code>using Notifier = std::function<void()>;</code>
    methods:
        - return: ' '
          name: Constructor
          param: [  ' DataSocket& stream', ' Notifier noAvailableData', ' Notifier flushing', ' std::vector<char>&& bufData', ' char const* currentStart', ' char const* currentEnd' ]
          mark:  
        - return: ' '
          name: Constructor
          param: [  ' DataSocket& stream', ' Notifier noAvailableData', ' Notifier flushing' ]
          mark:  
        - return: ' '
          name: Constructor
          param: [  ' DataSocket& stream' ]
          mark:  
        - return: ' '
          name: Move-Constructor
          param: [  ' ISocketStream&& move' ]
          mark:  noexcept
    protected: []
    virtual: []
children:
    - name: Constructor
      value: ThorsNisseCoreSocket.ISocketStream.methods.1.md
    - name: Constructor
      value: ThorsNisseCoreSocket.ISocketStream.methods.2.md
    - name: Constructor
      value: ThorsNisseCoreSocket.ISocketStream.methods.3.md
    - name: Move-Constructor
      value: ThorsNisseCoreSocket.ISocketStream.methods.4.md
---
