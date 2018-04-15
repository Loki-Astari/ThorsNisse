---
layout: method
generate: false
methodInfo:
    parent: HandlerSuspendable
    name: eventActivate
    mark:  final
    description:  |
        On first call will start the method `eventActivateNonBlocking()`. This method may suspend itself by calling `suspend()`.
        On subsequent calls will resume `eventActivateNonBlocking()` at the point where `suspend()` was called.
        If `eventActivateNon Blocking()` returns true then `dropHandler()` is called to remove this handler as processing is complete.
    return:
        type: 'short'
        description: 'This method returns the type of socket event that should be listened for in the main event loop.'
    parameters:
        - name: sockId
          type: 'LibSocketId'
          default: 
          description: 
        - name: eventType
          type: 'short'
          default: 
          description: 
---
