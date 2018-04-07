---
layout: method
generate: false
methodInfo:
    parent: HandlerBase
    name: addHandler
    description: If the handler wants to create other handlers.<br>An example of this is when the SQL handlers are added. They create connections to the SQL server that require their own event handlers.
    parameters:
        - name: 'args'
          type: 'Args&&...'
          description: Arguments passed to the constructor of the new handler
---

