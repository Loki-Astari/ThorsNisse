---
layout: method
generate: false
methodInfo:
    parent: HandlerBase
    name: eventActivate
    mark:  = 0
    description:  |
        Method called when there is data on the socket.
    return:
        type: 'short'
        description: 'Return the type of event that you can next process on this stream- EV_READ or EV_WRITE or (EV_READ | EV_WITE)'
    parameters:
        - name: sockId
          type: 'LibSocketId'
          default: 
          description: 'The socket with data available on it.'
        - name: eventType
          type: 'short'
          default: 
          description: 'The type of event that caused this handler to be triggered (EV_READ | EV_WRITE)'
---
