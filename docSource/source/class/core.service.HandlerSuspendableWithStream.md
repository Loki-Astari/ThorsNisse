---
layout: class
generate: false
typeInfo:
    namespace: ThorsAnvil::Nisse::Core::Service
    header:    ThorsNisseCoreSocket/Handler.h
    classname: HandlerSuspendableWithStream
    parent:    HandlerSuspendable
    description: An implementation of `eventActivateNonBlocking()` that creates input and output stream objects.<br>These stream objects will call `suspend()` if they are about to perform a blocking operation on the underlying socket.<br><br>Thus we have transparently non-blocking streams.
    methods:
        - return:
          name: Constructor
          param: ['Server&', 'LibSocketId', 'short']
          mark:
        - return: bool
          name: eventActivateNonBlocking
          param: []
          mark:
    virtual:
        - return: bool
          name:  eventActivateWithStream
          param: ['std::istream&', 'std::ostream&']
          mark:
---

