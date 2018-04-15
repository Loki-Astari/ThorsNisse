---
layout: method
generate: false
methodInfo:
    parent: DataSocket
    name: getMessageData
    mark:  
    description:  |
        Reads data from a sokcet into a buffer.
        If the stream is blocking will not return until the requested amount of data has been read or there is no more data to read.
        If the stream in non blocking will return if the read operation would block.
    return:
        type: 'std::pair<bool, std::size_t>'
        description: 'This method returns a <code>std::pair&lt;bool, std::size_t&gt;</code>. The first member `bool` indicates if more data can potentially be read from the stream. If the socket was cut or the EOF reached then this value will be false. The second member `std::size_t` indicates exactly how many bytes were read from this stream.'
    parameters:
        - name: buffer
          type: 'char*'
          default: 
          description: 'The buffer data will be read into. The size of the buffer. Offset into buffer (and amount size is reduced by) as this amount was read on a previous call).'
        - name: size
          type: 'std::size_t'
          default: 
          description: 'This method returns a <code>std::pair&lt;bool, std::size_t&gt;</code>. The first member `bool` indicates if more data can potentially be read from the stream. If the socket was cut or the EOF reached then this value will be false. The second member `std::size_t` indicates exactly how many bytes were read from this stream. The size of the buffer. Offset into buffer (and amount size is reduced by) as this amount was read on a previous call).'
        - name: alreadyGot
          type: 'std::size_t'
          default:  0
          description: 'Offset into buffer (and amount size is reduced by) as this amount was read on a previous call).'
---
