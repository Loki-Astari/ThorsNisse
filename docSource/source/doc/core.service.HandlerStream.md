---
layout: type
generate: false
typeInfo:
    namespace: ThorsAnvil::Nisse::Core::Service
    header:    ThorsNisseCoreSocket/Handler.h
    classname: HandlerStream
    parent:    HandlerBase
    description: This class is templatized based on the type of stream the socket represents.<br>The class basically defines a common class for holding the stream object and how to close it when requried.
    methods:
        - return: 
          name: Constructor
          param: ['Server&', 'LibSocketId', 'short']
          mark:
        - return: void
          name: close
          param: []
          mark:
---

