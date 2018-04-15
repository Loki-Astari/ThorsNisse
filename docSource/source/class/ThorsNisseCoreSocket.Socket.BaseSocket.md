---
layout: class
generate: false
typeInfo:
    namespace: ThorsAnvil::Nisse::Core::Socket
    header:    ThorsNisseCoreSocket/Socket.h
    classname: BaseSocket
    parent:    
    description:  |
        Base of all the socket classes.
        This class should not be directly created.
        All socket classes are movable but not copyable.
    methods:
        - return: ' '
          name: Move-Constructor
          param: [  ' BaseSocket&& move' ]
          mark:  noexcept
        - return: ' BaseSocket&'
          name: Move-Assignment
          param: [  ' BaseSocket&& move' ]
          mark:  noexcept
        - return: ' void'
          name: swap
          param: [  ' BaseSocket& other' ]
          mark:  noexcept
        - return: ' void'
          name: close
          param: [  ]
          mark: 
    protected: []
    virtual: []
children:
    - name: Move-Constructor
      value: ThorsNisseCoreSocket.Socket.BaseSocket.methods.1.Move-Constructor.md
    - name: Move-Assignment
      value: ThorsNisseCoreSocket.Socket.BaseSocket.methods.2.Move-Assignment.md
    - name: swap
      value: ThorsNisseCoreSocket.Socket.BaseSocket.methods.3.swap.md
    - name: close
      value: ThorsNisseCoreSocket.Socket.BaseSocket.methods.4.close.md
---
