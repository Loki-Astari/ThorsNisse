---
layout: method
generate: false
methodInfo:
    parent: ServerSocket
    name: accept
    mark:  
    description:  |
        "Accept" a waiting connection request on the port and creates a two way socket for communication on another port.
        If this is a non blocking socket and there is no waiting connection an exception is thrown.
        If this is a blocking socket wait for a connection.
    return:
        type: ' DataSocket'
        description: 'A <code>DataSocket</code> is returned so data can be exchange across the socket. Passed to the constructor of the <code>DataSocket</code> that is returned.'
    parameters:
        - name: blocking
          type: 'bool'
          default:  false
          description: 'Passed to the constructor of the <code>DataSocket</code> that is returned.'
---
