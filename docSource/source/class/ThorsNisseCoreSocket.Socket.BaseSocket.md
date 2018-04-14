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
      value: ThorsNisseCoreSocket.BaseSocket.methods.1.md
    - name: Move-Assignment
      value: ThorsNisseCoreSocket.BaseSocket.methods.2.md
    - name: swap
      value: ThorsNisseCoreSocket.BaseSocket.methods.3.md
    - name: close
      value: ThorsNisseCoreSocket.BaseSocket.methods.4.md
---
