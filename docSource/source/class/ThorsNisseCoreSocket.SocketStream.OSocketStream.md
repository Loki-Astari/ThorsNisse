---
layout: class
generate: false
typeInfo:
    namespace: ThorsAnvil::Nisse::Core::Socket
    header:    ThorsNisseCoreSocket/SocketStream.h
    classname: OSocketStream
    parent:    std::ostream
    description:  |
        An implementation of <code>std::istream</code> that uses <code>SocketStreamBuffer</code> as the buffer.
        The <code>Notofer</code> is a primitive event callback mechanism.
        A blocking read call to these streams calls the <code>Notifier noData</code>.cw
        This is used by the <a href="#Server">Server</a> infrastructure to yield control back to the main event loop.
        <code>using Notifier = std::function<void()>;</code>
    methods:
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
          param: [  ' OSocketStream&& move' ]
          mark:  noexcept
    protected: []
    virtual: []
children:
    - name: Constructor
      value: ThorsNisseCoreSocket.OSocketStream.methods.1.md
    - name: Constructor
      value: ThorsNisseCoreSocket.OSocketStream.methods.2.md
    - name: Move-Constructor
      value: ThorsNisseCoreSocket.OSocketStream.methods.3.md
---
