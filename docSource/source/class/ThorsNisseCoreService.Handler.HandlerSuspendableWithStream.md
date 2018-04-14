---
layout: class
generate: false
typeInfo:
    namespace: ThorsAnvil::Nisse::Core::Service
    header:    ThorsNisseCoreService/Handler.h
    classname: HandlerSuspendableWithStream
    parent:    HandlerSuspendable
    description:  |
        An implementation of `eventActivateNonBlocking()` that creates input and output stream objects.
        These stream objects will call `suspend()` if they are about to perform a blocking operation on the underlying socket.
        
        Thus we have transparently non-blocking streams.
    methods: []
    protected: []
    virtual:
        - return: ' bool'
          name: eventActivateNonBlocking
          param: [  ]
          mark:  final
        - return: ' bool'
          name: eventActivateWithStream
          param: [  ' std::istream& input', ' std::ostream& output' ]
          mark:  = 0
children:
    - name: eventActivateNonBlocking
      value: ThorsNisseCoreService.HandlerSuspendableWithStream.virtual.1.md
    - name: eventActivateWithStream
      value: ThorsNisseCoreService.HandlerSuspendableWithStream.virtual.2.md
---
