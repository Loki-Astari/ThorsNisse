---
layout: method
generate: false
methodInfo:
    parent: HandlerSuspendable
    name: eventActivate
    description: On first call will start the method `eventActivateNonBlocking()`. This method may suspend itself by calling `suspend()`.<br>On subsequent calls will resume `eventActivateNonBlocking()` at the point where `suspend()` was called.<br>If `eventActivateNonBlocking()` returns true then `dropHandler()` is called to remove this handler as processing is complete.<br>
    return:
        type:  short
        description: This method returns the type of socket event that should be listend for in the main event loop. 
    parameters: []
---

