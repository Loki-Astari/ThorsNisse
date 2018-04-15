---
layout: class
generate: false
typeInfo:
    namespace: ThorsAnvil::Nisse::Core::Service
    header:    ThorsNisseCoreService/Handler.h
    classname: HandlerBase
    parent:    
    description: 
    methods:
        - return: ' '
          name: Constructor
          param: [  ' Server& parent', ' LibSocketId socketId', ' short eventType', ' double timeout = 0' ]
          mark:  
    protected:
        - return: ' void'
          name: dropHandler
          param: [  ]
          mark: 
        - return: ' void'
          name: addHandler
          param: [  ' Args&&... args' ]
          mark:  
        - return: ' void'
          name: moveHandler
          param: [  ' Args&&... args' ]
          mark:  
    virtual:
        - return: ' short'
          name: eventActivate
          param: [  ' LibSocketId sockId', ' short eventType' ]
          mark:  = 0
        - return: ' bool '
          name: suspendable
          param: [  ]
          mark:  = 0
        - return: ' void '
          name: close
          param: [  ]
          mark:  = 0
        - return: ' void'
          name: suspend
          param: [  ' short type' ]
          mark:  = 0
children:
    - name: Constructor
      value: ThorsNisseCoreService.Handler.HandlerBase.methods.1.Constructor.md
    - name: dropHandler
      value: ThorsNisseCoreService.Handler.HandlerBase.protected.1.dropHandler.md
    - name: addHandler
      value: ThorsNisseCoreService.Handler.HandlerBase.protected.2.addHandler.md
    - name: moveHandler
      value: ThorsNisseCoreService.Handler.HandlerBase.protected.3.moveHandler.md
    - name: eventActivate
      value: ThorsNisseCoreService.Handler.HandlerBase.virtual.1.eventActivate.md
    - name: suspendable
      value: ThorsNisseCoreService.Handler.HandlerBase.virtual.2.suspendable.md
    - name: close
      value: ThorsNisseCoreService.Handler.HandlerBase.virtual.3.close.md
    - name: suspend
      value: ThorsNisseCoreService.Handler.HandlerBase.virtual.4.suspend.md
---
