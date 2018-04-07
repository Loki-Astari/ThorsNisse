---
layout: type
generate: false
typeInfo:
    namespace: ThorsAnvil::Nisse::Core::Service
    header:    ThorsNisseCoreSocket/Handler.h
    classname: HandlerBase
    parent:
    description:
    methods:
        - return:
          name: Constructor
          param: ['Server', 'LibSocketId', 'short', 'double']
          mark:
    protected:
        - return: void
          name: dropHAndler
          param: []
          mark:
        - return: void
          name: addHandler
          param: ['args&&...']
          mark:
        - return: void
          name: moveHandler
          param: ['args&&...']
          mark:
    virtual:
        - return: short
          name: eventActivate
          param: ['LibSocketId', 'short']
          mark:
        - return: bool
          name: suspendable
          param: []
          mark:
        - return: void
          name: suspend
          param: ['short']
          mark:
        - return: void
          name: close
          param: []
          mark:
children:
    - name: HandlerBase::dropHandler
      value: core.service.HandlerBase.dropHandler.md
    - name: HandlerBase::addHandler
      value: core.service.HandlerBase.addHandler.md
    - name: HandlerBase::moveHandler
      value: core.service.HandlerBase.moveHandler.md
    - name: HandlerBase::eventActivate
      value: core.service.HandlerBase.eventActivate.md
    - name: HandlerBase::suspendable
      value: core.service.HandlerBase.suspendable.md
    - name: HandlerBase::suspend
      value: core.service.HandlerBase.suspend.md
    - name: HandlerBase::close
      value: core.service.HandlerBase.close.md
    - name: HandlerBase::eventActivateNonBlocking
      value: core.service.HandlerBase.eventActivateNonBlocking.md
---

