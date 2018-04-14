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
      value: ThorsNisseCoreService.HandlerBase.methods.1.md
    - name: dropHandler
      value: ThorsNisseCoreService.HandlerBase.protected.1.md
    - name: addHandler
      value: ThorsNisseCoreService.HandlerBase.protected.2.md
    - name: moveHandler
      value: ThorsNisseCoreService.HandlerBase.protected.3.md
    - name: eventActivate
      value: ThorsNisseCoreService.HandlerBase.virtual.1.md
    - name: suspendable
      value: ThorsNisseCoreService.HandlerBase.virtual.2.md
    - name: close
      value: ThorsNisseCoreService.HandlerBase.virtual.3.md
    - name: suspend
      value: ThorsNisseCoreService.HandlerBase.virtual.4.md
---
