---
layout: method
generate: false
methodInfo:
    parent: DataSocket
    name: getMessageData
    description: Reads data from a sokcet into a buffer.<br>If the stream is blocking will not return until the requested amount of data has been read or there is no more data to read.<br>If the stream in non blocking will return if the read operation would block.
    return:
        type: std::pair&lt;bool, std::size_t&gt;
        description: This method returns a <code>std::pair&lt;bool, std::size_t&gt;</code><br>The first member `bool` indicates if more data can potentially be read from the stream. If the socket was cut or the EOF reached then this value will be false<br>The second member `std::size_t` indicates exactly how many bytes were read from this stream.
    parameters:
        - name: buffer
          type: char*
          description: The buffer data will be read into.
        - name: size
          type: std::size_t
          description: The size of the buffer.
        - name: alreadyGot
          type: std::size_t
          description: Offset into buffer (and amount size is reduced by) as this amount was read on a previous call).
---

