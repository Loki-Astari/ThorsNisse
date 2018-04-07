---
layout: type
generate: false
typeInfo:
    namespace: ThorsAnvil::Nisse::Core::Service
    header:    ThorsNisseCoreSocket/Server.h
    classname: Server
    parent:
    description:
    methods:
        - return:
          name: Default Constructor
          param: []
          mark:
        - return:
          name: Move Constructor
          param: []
          mark:
        - return:
          name: Move Assignment
          param: []
          mark:
        - return: void
          name: start
          param: ['double']
          mark:
        - return: void
          name: flagShutDown
          param: []
          mark:
        - return: void
          name: listenOn
          param: ['ServerConnection const&', 'Param']
          mark:
        - return: void
          name: addTimer
          param: ['double', 'std::function<void()>&&']
          mark:
children:
    - name: Server::start
      value: core.service.Server.start.md
    - name: Server::flagShutDown
      value: core.service.Server.flagShutDown.md
    - name: Server::listenOn
      value: core.service.Server.listenOn.md
    - name: Server::addTimer
      value: core.service.Server.addTimer.md
---

