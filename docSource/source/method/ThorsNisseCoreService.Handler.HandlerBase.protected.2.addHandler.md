---
layout: method
generate: false
methodInfo:
    parent: HandlerBase
    name: addHandler
    mark:  
    description:  |
        If the handler wants to create other handlers.
        An example of this is when the SQL handlers are added. They create connections to the SQL server that require their own event handlers.
    return:
        type: ' void'
        description: ''
    parameters:
        - name: args
          type: 'Args&&...'
          default: 
          description: 'Arguments passed to the constructor of the new handler.'
---
