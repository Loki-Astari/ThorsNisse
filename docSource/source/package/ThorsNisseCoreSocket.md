---
layout: package
generate: false
nameSpace:  ThorsAnvil::Nisse::Core::Socket
headers:
    base: ThorsNisseCoreSocket
    files:
        - name:   Socket.h
          functions: []
          classes:
            - name:   BaseSocket
              parent: 
            - name:   DataSocket
              parent: BaseSocket
            - name:   ConnectSocket
              parent: DataSocket
            - name:   ServerSocket
              parent: BaseSocket
        - name:   SocketStream.h
          functions: []
          classes:
            - name:   SocketStreamBuffer
              parent: std::streambuf
            - name:   ISocketStream
              parent: std::istream
            - name:   OSocketStream
              parent: std::ostream
children:
    - name: BaseSocket
      value: ThorsNisseCoreSocket.Socket.BaseSocket.md
    - name: DataSocket
      value: ThorsNisseCoreSocket.Socket.DataSocket.md
    - name: ConnectSocket
      value: ThorsNisseCoreSocket.Socket.ConnectSocket.md
    - name: ServerSocket
      value: ThorsNisseCoreSocket.Socket.ServerSocket.md
    - name: SocketStreamBuffer
      value: ThorsNisseCoreSocket.SocketStream.SocketStreamBuffer.md
    - name: ISocketStream
      value: ThorsNisseCoreSocket.SocketStream.ISocketStream.md
    - name: OSocketStream
      value: ThorsNisseCoreSocket.SocketStream.OSocketStream.md
---

```bash
 > g++ Socket.cpp -o Socket -I${THOR_ROOT}/include -L${THOR_ROOT}/lib -lThorsExpress17
 > curl --data "A test message in a bottle :)" http://localhost:8080;echo
```
```cpp
// Server Side
#include "ThorsNisseCoreSocket/Socket.h"
#include "ThorsNisseCoreSocket/SocketStream.h"
#include <string>
int main()
{
    namespace Sock = ThorsAnvil::Nisse::Core::Socket;

    Sock::ServerSocket    server(8080);
    while(true)
    {
        Sock::DataSocket    connection = server.accept();
        Sock::ISocketStream input(connection);

        std::string     request;
        std::getline(input, request);

        std::string     header;
        std::size_t     bodySize = 0;
        while(std::getline(input, header) && header != "\r")
        {
            /*
             * Note: This code is still handling the intricacy of the HTTP protocol
             *       so it is brittle. See Protocol/HTTP for help with handling the
             *       details of the protocol.
             */
            if (header.compare(0, 15, "Content-Length:") == 0) {
                bodySize = std::stoi(header.substr(15));
            }
        }

        std::string     message;
        std::string     line;
        while(bodySize > 0 && std::getline(input, line))
        {
            message += line;
            message += "<br>";
            bodySize -= (line.size() + 1);
        }

        Sock::OSocketStream output(connection);
        /*
         * Note: This code is still handling the intricacy of the HTTP protocol
         *       so it is brittle. See Protocol/HTTP for help with handling the
         *       details of the protocol.
         */
        output << "HTTP/1.1 200 OK\r\n"
                  "Content-Length: " << (11 + message.size()) << "\r\n"
                  "\r\n"
                  "It Worked: " << message;
    }
}
```

