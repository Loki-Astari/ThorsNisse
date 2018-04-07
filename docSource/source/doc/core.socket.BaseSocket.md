---
layout: type
generate: false
typeInfo:
    namespace: ThorsAnvil::Nisse::Core::Socket
    header:    ThorsNisseCoreSocket/Socket.h
    classname: BaseSocket
    parent:
    description: Base of all the socket classes.<br>This class should not be directly created.<br>All socket classes are movable but not copyable.
    methods:
        - return:
          name: Move Constructor
          param: []
          mark: noexcept
        - return: BaseSocket&
          name: Move Assignment
          param: []
          mark: noexcept
        - return: void
          name: swap
          param: []
          mark: noexcept
        - return: void
          name: close
          param: []
          mark:
---

