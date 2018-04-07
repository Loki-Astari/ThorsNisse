---
layout: method
generate: false
methodInfo:
    parent: HandlerBase
    name: eventActivateNonBlocking
    description: Called by eventActivate() to start processing of the stream.<br>If a blocking operation is about to be performed this method should call `suspend()` to return control to the main event loop.
    return:
        type: bool
        description: Return true to cause the handler to be re-used.<br>Return false to not drop the handler. This is used if something clever is happening.
    parameters: []
---

