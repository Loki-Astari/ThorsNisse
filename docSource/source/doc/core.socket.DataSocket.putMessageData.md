---
layout: method
generate: false
methodInfo:
    parent: DataSocket
    name: putMessageData
    description: Writes data from a buffer to a sokcet.<br>If the stream is blocking will not return until the requested amount of data has been written or the socket was closed to writting.<br>If the stream in non blocking will return if the write operation would block.
    return:
        type: std::pair&lt;bool, std::size_t&gt;
        description: This method returns a <code>std::pair&lt;bool, std::size_t&gt;</code><br>The first member `bool` indicates if more data can potentially be written to the stream. If the socket was cut or closed then this value will be false<br>The second member `std::size_t` indicates exactly how many bytes were written to this stream.
    parameters:
        - name: buffer
          type: char*
          description: The buffer data will be written from.
        - name: size
          type: std::size_t
          description: The size of the buffer.
        - name: alreadyPut
          type: std::size_t
          description: Offset into buffer (and amount size is reduced by) as this amount was written on a previous call).
---

