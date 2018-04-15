---
layout: method
generate: false
methodInfo:
    parent: Server
    name: flagShutDown
    mark:  
    description:  |
        Marks the event loop for shut down.
        After the current iteration of the event loop has finished it will exit. This will cause the `start()` function to return.
    return:
        type: 'void'
        description: 
    parameters: []
---
