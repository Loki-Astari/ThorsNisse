---
layout: method
generate: false
methodInfo:
    parent: ServerSocket
    name: accept
    description: \"Accept\" a waiting connection request on the port and creates a two way socket for communication on another port.<br>If this is a non blocking socket and there is no waiting connection an exception is thrown.<br>If this is a blocking socket wait for a connection<br>
    return:
        type: DataSocket
        description: A <code>DataSocket</code> is returned so data can be exchange across the socket.
    parameters:
        - name: blocking
          type: bool
          description: Passed to the constructor of the <code>DataSocket</code> that is returned.
---

