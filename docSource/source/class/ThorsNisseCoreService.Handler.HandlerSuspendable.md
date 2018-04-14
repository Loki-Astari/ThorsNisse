---
layout: class
generate: false
typeInfo:
    namespace: ThorsAnvil::Nisse::Core::Service
    header:    ThorsNisseCoreService/Handler.h
    classname: HandlerSuspendable
    parent:    HandlerStream
    description:  |
        Defines a handler that is suspendable.
        Implements `suspendable`, `suspsend()` and `eventActivate()` as these all work together to define a class that can be suspended.
        The method `eventActivateNonBlocking()` should be overridden by derived classes to provide functionally.
    methods:
        - return: ' '
          name: Constructor
          param: [  ' Server& parent', ' Stream&& stream', ' short eventType' ]
          mark:  
        - return: ' '
          name: Constructor
          param: [  ' Server& parent', ' Stream&& stream', ' short eventType', ' short firstEvent' ]
          mark:  
    protected: []
    virtual:
        - return: ' void'
          name: suspend
          param: [  ' short type' ]
          mark:  final 
        - return: ' bool'
          name: suspendable
          param: [  ]
          mark:  final
        - return: ' short'
          name: eventActivate
          param: [  ' LibSocketId sockId', ' short eventType' ]
          mark:  final
        - return: ' bool'
          name: eventActivateNonBlocking
          param: [  ]
          mark:  = 0
children:
    - name: Constructor
      value: ThorsNisseCoreService.HandlerSuspendable.methods.1.md
    - name: Constructor
      value: ThorsNisseCoreService.HandlerSuspendable.methods.2.md
    - name: suspend
      value: ThorsNisseCoreService.HandlerSuspendable.virtual.1.md
    - name: suspendable
      value: ThorsNisseCoreService.HandlerSuspendable.virtual.2.md
    - name: eventActivate
      value: ThorsNisseCoreService.HandlerSuspendable.virtual.3.md
    - name: eventActivateNonBlocking
      value: ThorsNisseCoreService.HandlerSuspendable.virtual.4.md
---
