---
layout: method
generate: false
methodInfo:
    parent: HandlerBase
    name: dropHandler
    mark:  
    description:  |
        When a handler is finished processing events on a stream. It can call dropHandler() to remove itself from the event loop.
        This is usually done automatically by higher level derived handlers.
    return:
        type: ' void'
        description: ''
    parameters: []
---
