---
layout: method
generate: false
methodInfo:
    parent: Server
    name: start
    mark:  
    description:  |
        Starts the event loop.
        This method does not return immediately. A call to <code>flagShutDown()</code> will cause the event loop to exit after the current iteration.
    return:
        type: 'void'
        description: 
    parameters:
        - name: check
          type: 'double'
          default:  10.0
          description: 
---
