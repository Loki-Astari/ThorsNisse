---
layout: class
generate: false
typeInfo:
    namespace: ThorsAnvil::Nisse::Core::Service
    header:    ThorsNisseCoreService/Server.h
    classname: Server
    parent:    
    description:  |
        An object that acts as the main server event loop.
        One of these objects can handle all the ports your application requires
    methods:
        - return: ' '
          name: Default-Constructor
          param: [  ]
          mark: 
        - return: ' '
          name: Move-Constructor
          param: [  ' Server&&' ]
          mark:  
        - return: ' Server&'
          name: Move-Assignment
          param: [  ' Server&&' ]
          mark:  
        - return: ' void'
          name: start
          param: [  ' double check = 10.0' ]
          mark:  
        - return: ' void'
          name: flagShutDown
          param: [  ]
          mark: 
        - return: ' void'
          name: listenOn
          param: [  ' ServerConnection const& info', ' Param& param' ]
          mark:  
        - return: ' void'
          name: addTimer
          param: [  ' double timeOut', ' std::function<void()>&& action' ]
          mark:  
    protected: []
    virtual: []
children:
    - name: Default-Constructor
      value: ThorsNisseCoreService.Server.methods.1.md
    - name: Move-Constructor
      value: ThorsNisseCoreService.Server.methods.2.md
    - name: Move-Assignment
      value: ThorsNisseCoreService.Server.methods.3.md
    - name: start
      value: ThorsNisseCoreService.Server.methods.4.md
    - name: flagShutDown
      value: ThorsNisseCoreService.Server.methods.5.md
    - name: listenOn
      value: ThorsNisseCoreService.Server.methods.6.md
    - name: addTimer
      value: ThorsNisseCoreService.Server.methods.7.md
---
