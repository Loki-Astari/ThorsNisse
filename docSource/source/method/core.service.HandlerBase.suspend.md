---
layout: method
generate: false
methodInfo:
    parent: HandlerBase
    name: suspend
    description: Suspend the execution of this class until `type` (EV_READ | EV_WRITE) is available for this socket.<br>This basically returns control to the main event loop until data is available. This is used by the stream handlers to return control to the main event loop if they would block when reading     from a stream.
    parameters:
        - name: type
          type: short
          description: the type of event we are wiaitng for
---

