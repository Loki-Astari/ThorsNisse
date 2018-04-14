---
layout: class
generate: false
typeInfo:
    namespace: ThorsAnvil::Nisse::Core::Service
    header:    ThorsNisseCoreSocket/Handler.h
    classname: HandlerNonSuspendable
    parent:    HandlerStream
    description: Defines `suspendable()` and `suspend()` for a class that is non suspendable.<br>This is used by some of ther server built in handlers that must complete. It is unlikely that this will be useful for a user defined handler.
    methods:
        - return:
          name: Constructor
          param: ['Server&', 'LibSocketId', 'short']
          mark:
        - return: bool
          name:  suspendable
          param: []
          mark:
        - return: void
          name: suspend
          param: ['short']
          mark:
---

