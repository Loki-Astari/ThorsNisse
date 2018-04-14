---
layout: method
generate: false
methodInfo:
    parent: HandlerBase
    name: moveHandler
    mark:  
    description:  |
        This is similar to `addHandler()`.
        The difference is that the current handler will immediately suspend until the created handler complets.
        When the added handler calls `dropHandler()` control will be returned to the current handler at the point it suspended.
        Note- The current handler must be suspendable (otherwise an exception is thrown).See-  <a href="HandlerBase::suspendable">HandlerBase::suspendable</a>
    return:
        type: ' void'
        description: ''
    parameters:
        - name: args
          type: 'Args&&...'
          default: 
          description: 'Arguments passed to the constructor of the new handler'
---
