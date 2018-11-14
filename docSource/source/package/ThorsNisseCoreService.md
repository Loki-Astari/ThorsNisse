---
layout: package
generate: false
nameSpace:  ThorsAnvil::Nisse::Core::Service
headers:
    base: ThorsNisseCoreService
    files:
        - name:   Handler.h
          functions: []
          classes:
            - name:   HandlerBase
              parent: 
            - name:   HandlerStream
              parent: HandlerBase
            - name:   HandlerNonSuspendable
              parent: HandlerStream<Stream>
            - name:   HandlerSuspendable
              parent: HandlerStream<Stream>
            - name:   HandlerSuspendableWithStream
              parent: HandlerSuspendable<Socket::DataSocket>
        - name:   Server.h
          functions: []
          classes:
            - name:   ServerConnection
              parent: 
            - name:   Server
              parent: 
        - name:   ServerHandler.h
          functions: []
          classes:
            - name:   ServerHandler
              parent: HandlerNonSuspendable<Socket::ServerSocket>
            - name:   TimerHandler
              parent: HandlerNonSuspendable<int>
children:
    - name: HandlerBase
      value: ThorsNisseCoreService.Handler.HandlerBase.md
    - name: HandlerStream
      value: ThorsNisseCoreService.Handler.HandlerStream.md
    - name: HandlerNonSuspendable
      value: ThorsNisseCoreService.Handler.HandlerNonSuspendable.md
    - name: HandlerSuspendable
      value: ThorsNisseCoreService.Handler.HandlerSuspendable.md
    - name: HandlerSuspendableWithStream
      value: ThorsNisseCoreService.Handler.HandlerSuspendableWithStream.md
    - name: ServerConnection
      value: ThorsNisseCoreService.Server.ServerConnection.md
    - name: Server
      value: ThorsNisseCoreService.Server.Server.md
    - name: ServerHandler
      value: ThorsNisseCoreService.ServerHandler.ServerHandler.md
    - name: TimerHandler
      value: ThorsNisseCoreService.ServerHandler.TimerHandler.md
---

```bash
 > g++ -std=c++1z Server.cpp -o Server -I${THOR_ROOT}/include -L${THOR_ROOT}/lib -lThorsExpress17 -lboost_context-mt
 > curl --data "A test message in a bottle :)" http://localhost:8080;echo
```
```cpp
#include "ThorsNisseCoreService/Server.h"
#include "ThorsNisseCoreService/Handler.h"
#include "ThorsNisseCoreSocket/Socket.h"
#include "ThorsNisseCoreSocket/SocketStream.h"
#include <string>

namespace Serv = ThorsAnvil::Nisse::Core::Service;
namespace Sock = ThorsAnvil::Nisse::Core::Socket;

class MyHandler: public Serv::HandlerSuspendableWithStream
{
    public:
        MyHandler(Serv::Server& parent, Sock::DataSocket&& stream)
            : HandlerSuspendableWithStream(parent, std::move(stream), EV_READ)
        {}
        virtual bool eventActivateWithStream(std::istream& input, std::ostream& output)
        {
            /*
             * Note: The streams `input` and `output` are transparently non-blocking.
             *       Any blocking read/write will transfer control back to the server
             *       event loop.
             *
             * To the code in this function any read/write requests appear to be blocking
             * So the writting code is still simple and looks like standard C++ code.
             */
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

            /*
             * Note: This code is still handling the intricacy of the HTTP protocol
             *       so it is brittle. See Protocol/HTTP for help with handling the
             *       details of the protocol.
             */
            output << "HTTP/1.1 200 OK\r\n"
                      "Content-Length: " << (11 + message.size()) << "\r\n"
                      "\r\n"
                      "It Worked: " << message;
            return true;
        }
};
int main()
{
    Serv::Server      server;
    server.listenOn<MyHandler>(8080);
    server.listenOn<MyHandler>(Serv::ServerConnection(8081, 20));

    // Start the event loop.
    server.start();
}
```
A simple wrapper around libEvent.

