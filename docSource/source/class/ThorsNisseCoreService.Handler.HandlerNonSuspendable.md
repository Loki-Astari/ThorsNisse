---
layout: class
generate: false
typeInfo:
    namespace: ThorsAnvil::Nisse::Core::Service
    header:    ThorsNisseCoreService/Handler.h
    classname: HandlerNonSuspendable
    parent:    HandlerStream
    description:  |
        Defines `suspendable()` and `suspend()` for a class that is non suspendable.
        This is used by some of there server built in handlers that must complete. It is unlikely that this will be useful for a user defined handler.
    methods: []
    protected: []
    virtual:
        - return: ' void'
          name: suspend
          param: [  ' short' ]
          mark:  final 
        - return: ' bool'
          name: suspendable
          param: [  ]
          mark:  final
children:
    - name: suspend
      value: ThorsNisseCoreService.HandlerNonSuspendable.virtual.1.md
    - name: suspendable
      value: ThorsNisseCoreService.HandlerNonSuspendable.virtual.2.md
---
