---
layout: type
generate: false
typeInfo:
    namespace: ThorsAnvil::Nisse::Core::Service
    header:    ThorsNisseCoreSocket/Handler.h
    classname: HandlerSuspendable
    parent:    HandlerStream
    description: Defines a handler that is suspendable.<br>Implements `suspendable`, `suspsend()` and `eventActivate()` as these all work together to define a class that can be suspended.<br>The method `eventActivateNonBlocking()` should be overwridden by derived classes to provide functionaliy.
    methods:
        - return: 
          name:  Constructor
          param: ['Server&', 'LibSocketId', 'short']
          mark:
        - return: bool
          name: suspendable
          param: []
          mark:
        - return: void
          name: suspend
          param: ['short']
          mark:
        - return: short
          name: eventActivate
          param: ['LibSocketId', 'short']
          mark:
    virtual:
        - return: bool
          name:  eventActivateNonBlocking
          param: []
          mark:
children:
    - name: HandlerSuspendable::eventActivate
      value: core.service.HandlerSuspendable.eventActivate.md
---

