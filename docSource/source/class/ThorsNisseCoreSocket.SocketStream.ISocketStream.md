---
layout: class
generate: false
typeInfo:
    namespace: ThorsAnvil::Nisse::Core::Socket
    header:    ThorsNisseCoreSocket/Socket.h
    classname: ISocketStream
    parent:    std::istream
    description: An implementation of <code>std::istream</code> that uses <code>SocketStreamBuffer</code> as the buffer.<br>The <code>Notofer</code> is a primitive event callback mechanism.<br>A blocking read call to these streams calls the <code>Notifier noData</code>. This is used by the <a href="#Server">Server</a> infastructure to yield control back to the main event loop.<br><code>using Notifier = std::function<void()>;</code>
    methods:
        - return:
          name: Constructor
          param: ['DataSocket&', 'Notifier', 'Notifier']
          mark:
        - return: 
          name: Constructor
          param: ['DataSocket&']
          mark:
        - return:
          name: Move Constructor
          param: []
          mark: 
---