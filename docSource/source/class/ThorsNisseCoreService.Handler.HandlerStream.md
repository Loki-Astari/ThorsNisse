---
layout: class
generate: false
typeInfo:
    namespace: ThorsAnvil::Nisse::Core::Service
    header:    ThorsNisseCoreService/Handler.h
    classname: HandlerStream
    parent:    HandlerBase
    description:  |
        This class is templatized based on the type of stream the socket represents.
        The class basically defines a common class for holding the stream object and how to close it when required.
    methods:
        - return: ' '
          name: Constructor
          param: [  ' Server& parent', ' Stream&& stream', ' short eventType', ' double timeout = 0' ]
          mark:  
    protected: []
    virtual:
        - return: ' void '
          name: close
          param: [  ]
          mark:  override
children:
    - name: Constructor
      value: ThorsNisseCoreService.Handler.HandlerStream.methods.1.Constructor.md
    - name: close
      value: ThorsNisseCoreService.Handler.HandlerStream.virtual.1.close.md
---
