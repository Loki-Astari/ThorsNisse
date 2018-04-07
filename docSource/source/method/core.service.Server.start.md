---
layout: method
generate: false
methodInfo:
    parent: Server
    name: start
    description: Starts the event loop.<br>This method does not return immediately. A call to <code>flagShutDown()</code> will cause the event loop to exit after the current iteration.
    parameters:
        - name: check
          type: double
          description: Timeout period after which internal house keeping operations are performed.
---

