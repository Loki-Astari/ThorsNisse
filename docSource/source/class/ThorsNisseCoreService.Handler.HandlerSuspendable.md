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
      value: ThorsNisseCoreService.Handler.HandlerSuspendable.methods.1.Constructor.md
    - name: Constructor
      value: ThorsNisseCoreService.Handler.HandlerSuspendable.methods.2.Constructor.md
    - name: suspend
      value: ThorsNisseCoreService.Handler.HandlerSuspendable.virtual.1.suspend.md
    - name: suspendable
      value: ThorsNisseCoreService.Handler.HandlerSuspendable.virtual.2.suspendable.md
    - name: eventActivate
      value: ThorsNisseCoreService.Handler.HandlerSuspendable.virtual.3.eventActivate.md
    - name: eventActivateNonBlocking
      value: ThorsNisseCoreService.Handler.HandlerSuspendable.virtual.4.eventActivateNonBlocking.md
---
