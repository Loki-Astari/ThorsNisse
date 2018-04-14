---
layout: method
generate: false
methodInfo:
    parent: DataSocket
    name: putMessageData
    mark:  
    description:  |
        Writes data from a buffer to a sokcet.
        If the stream is blocking will not return until the requested amount of data has been written or the socket was closed to writting.
        If the stream in non blocking will return if the write operation would block.
    return:
        type: ' std::pair<bool, std::size_t>'
        description: 'This method returns a <code>std::pair&lt;bool, std::size_t&gt;</code>. The first member `bool` indicates if more data can potentially be written to the stream. If the socket was cut or closed then this value will be false. The second member `std::size_t` indicates exactly how many bytes were written to this stream.'
    parameters:
        - name: buffer
          type: 'char const*'
          default: 
          description: 'The buffer data will be written from. The size of the buffer. Offset into buffer (and amount size is reduced by) as this amount was written on a previous call).'
        - name: size
          type: 'std::size_t'
          default: 
          description: 'This method returns a <code>std::pair&lt;bool, std::size_t&gt;</code>. The first member `bool` indicates if more data can potentially be written to the stream. If the socket was cut or closed then this value will be false. The second member `std::size_t` indicates exactly how many bytes were written to this stream. The size of the buffer. Offset into buffer (and amount size is reduced by) as this amount was written on a previous call).'
        - name: alreadyPut
          type: 'std::size_t'
          default:  0
          description: 'Offset into buffer (and amount size is reduced by) as this amount was written on a previous call).'
---
