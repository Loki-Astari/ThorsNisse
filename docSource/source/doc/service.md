---
layout: article
generate: false
---

## Service
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
<dl>
<dt>NameSpace:</dt><dd>ThorsAnvil::Nisse::Core::Service</dd>
<dt>Headers:</dt><dd>ThorsNisseCoreService/</dd>
<dt>Server.h</dt><dd>

* struct ServerConnection
* class Server

</dd>
<dt>Handler.h</dt><dd>

* class HandlerBase
* class HandlerStream: public HandlerBase
* class HandlerNonSuspendable: public HandlerStream
* class HandlerSuspendable: public HandlerStream

</dd>
<dt>ServerHandlers.h</dt><dd>

* class ServerHandler: public HandlerNonSuspendable
* class TimerHandler: public HandlerNonSuspendable

</dd>
</dl>
### ServerConnection
```cpp
    Serv::Server      server;
    // Implicit creation of ServerConnection
    server.listenOn<MyHandler>(8080);
    // Explicit creation of ServerConnection
    server.listenOn<MyHandler>(Serv::ServerConnection(8081, 20));
```
<div class="classTable allowFloat">
    <div class="classLayout">
        <div class="classBlock">
            <div class="classRow"><div class="classItem">Header:   </div><div class="classValue">ThorsNisseCoreService/Server.h</div></div>
            <div class="classRow"><div class="classItem">NameSpace:</div><div class="classValue">ThorsAnvil::Nisse::Core::Service</div></div>
            <div class="classRow"><div class="classItem">Struct:   </div><div class="classValue">ServerConnection</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classDesc">
            </div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classSect">Public Members:</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="methodRet"></div><div class="methodName">ServerConnection(int port, int maxConnections)</div><div class="methodMark"></div></div>
            <div class="classRow"><div class="methodRet">int</div><div class="methodName">port</div><div class="methodMark"></div></div>
            <div class="classRow"><div class="methodRet">int</div><div class="methodName">maxConnections</div><div class="methodMark"></div></div>
        </div>
    </div>
</div>


### Server

<div class="classTable">
    <div class="classLayout">
        <div class="classBlock">
            <div class="classRow"><div class="classItem">Header:   </div><div class="classValue">ThorsNisseCoreService/Server.h</div></div>
            <div class="classRow"><div class="classItem">NameSpace:</div><div class="classValue">ThorsAnvil::Nisse::Core::Service</div></div>
            <div class="classRow"><div class="classItem">Class:    </div><div class="classValue">Server</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classDesc">
            </div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classSect">Public Members:</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="methodRet"></div><div class="methodName">Server()</div><div class="methodMark"></div></div>
            <div class="classRow"><div class="methodRet"></div><div class="methodName">Server(Server&&)</div><div class="methodMark"></div></div>
            <div class="classRow"><div class="methodRet"></div><div class="methodName">Server& operator=(Server&&)</div><div class="methodMark"></div></div>
            <div class="classRow"><div class="methodRet">void</div><div class="methodName">start(double check = 10.0)</div><div class="methodMark"></div></div>
            <div class="classRow"><div class="methodRet">void</div><div class="methodName">flagShutDown()</div><div class="methodMark"></div></div>
            <div class="classRow"><div class="methodRet">void</div><div class="methodName">listenOn(ServerConnection const& info, Param& param)</div><div class="methodMark"></div></div>
            <div class="classRow"><div class="methodRet">void</div><div class="methodName">addTimer(double timeOut, std::function<void()>&& action)</div><div class="methodMark"></div></div>
        </div>
    </div>
</div>

#### Server::start

<div class="classTable">
    <div class="classLayout">
        <div class="classBlock">
            <div class="classRow"><div class="classSect">start(double check = 10.0)</div></div>
            <div class="classRow"><div class="classDesc">
            Starts the event loop.<br>
            This method does not return immediately. A call to <code>flagShutDown()</code> will cause the event loop to exit after the current iteration.
            </div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classSect">Parameters</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classItem">check</div><div class="classValue">Timeout period after which internal house keeping operations are performed.</div></div>
        </div>
    </div>
</div>
#### Server::flagShutDown

<div class="classTable">
    <div class="classLayout">
        <div class="classBlock">
            <div class="classRow"><div class="classSect">flagShutDown()</div></div>
            <div class="classRow"><div class="classDesc">
            Marks the event loop for shut down.<br>
            After the current iteration of the event loop has finished it will exit. This will cause the `start()` function to return.
            </div></div>
        </div>
    </div>
</div>
#### Server::listenOn
```cpp
class MyState
{
    // Stuf
};
class MyHandler: public Serv::HandlerSuspendableWithStream
{
    public:
        // Notice the last parameter is `state`.
        // This means a state object must be provided to the `listenOn()` method.
        MyHandler(Serv::Server& parent, Sock::DataSocket&& stream, MyState& state)
            : HandlerSuspendableWithStream(parent, std::move(stream), EV_READ)
        {}

        virtual bool eventActivateWithStream(std::istream& input, std::ostream& output)
        { // Stuff }
};
int main()
{
    Serv::Server      server;
    MyStateObject     state;

    // Because we pass state to the `listenOn()` method; this will be passed to the constructor of 
    // `MyHandler` when it is created. This allows a user defined state object to be used consistently
    // across all connections.
    server.listenOn<MyHandler>(8080, state);

    // Start the event loop.
    server.start();
}
```

<div class="classTable allowFloat">
    <div class="classLayout">
        <div class="classBlock">
            <div class="classRow"><div class="classSect">listenOn(int port, Param& data)</div></div>
            <div class="classRow"><div class="classDesc">
            This is a templatized function. The template type is the type of object that will handle a connection once it has been established.<br>
            When a connection is accepted and object of the template type is created and passed a reference to the server, stream and `data` (see parameters)<br>
            The object is marked as listening to the accepted socket and when data is available on the socket the <code>eventActivate()</code> method is called allowing the data to be processed.<br>
            For more details see <a href="#HandlerBase">HandlerBase</a>.
            </div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classSect">Parameters</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classItem">port</div><div class="classValue">The port that is being listen too.</div></div>
            <div class="classRow"><div class="classItem">data</div><div class="classValue">A reference to an object that is passed to the constructor of the handler type. This allows a state object to be passed to the constructor</div></div>
        </div>
    </div>
</div>
#### Server::addTimer
```cpp
int main()
{
    Serv::Server      server;
    MyStateObject     state;

    server.addTimer(3.5, [](){std::cout << "Timer\n";});

    // Start the event loop.
    // While the event loop is running the timer will be activated every 3.5 seconds
    server.start();
}
```

<div class="classTable allowFloat">
    <div class="classLayout">
        <div class="classBlock">
            <div class="classRow"><div class="classSect">addTimer(double timeOut, std::function<void()>&& action)</div></div>
            <div class="classRow"><div class="classDesc">
            Sets a timer to go off every `timeOut` seconds.<br>
            The result of the timmer going off is to execute the functot `action`.
            </div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classSect">Parameters</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classItem">timeOut</div><div class="classValue">The time period (in seconds)  between running the action object.</div></div>
            <div class="classRow"><div class="classItem">action</div><div class="classValue">Functor that is run every `timeOut` seconds..</div></div>
        </div>
    </div>
</div>
### HandlerBase

<div class="classTable allowFloat">
    <div class="classLayout">
        <div class="classBlock">
            <div class="classRow"><div class="classItem">Header:   </div><div class="classValue">ThorsNisseCoreService/Handler.h</div></div>
            <div class="classRow"><div class="classItem">NameSpace:</div><div class="classValue">ThorsAnvil::Nisse::Core::Service</div></div>
            <div class="classRow"><div class="classItem">Class:    </div><div class="classValue">HandlerBase</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classDesc">
            </div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classSect">Public Members:</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="methodRet"></div><div class="methodName">HandlerBase(Server& parent, LibSocketId socketId, short eventType, double timeout = 0)</div><div class="methodMark"></div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classSect">Protected Members:</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="methodRet">void</div><div class="methodName">dropHandler()</div><div class="methodMark"></div></div>
            <div class="classRow"><div class="methodRet">void</div><div class="methodName">addHandler(Args&&... args)</div><div class="methodMark"></div></div>
            <div class="classRow"><div class="methodRet">void</div><div class="methodName">moveHandler(Args&&... args)</div><div class="methodMark"></div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classSect">Virtual Members:</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="methodRet">short</div><div class="methodName">eventActivate(LibSocketId sockId, short eventType)</div><div class="methodMark"></div></div>
            <div class="classRow"><div class="methodRet">bool</div><div class="methodName">suspendable()</div><div class="methodMark"></div></div>
            <div class="classROw"><div class="methodRet">void</div><div class="methodNAme">suspend(short type)</div></div>
            <div class="classRow"><div class="methodRet">void</div><div class="methodName">close()</div><div class="methodMark"></div></div>
        </div>
    </div>
</div>
#### HandlerBase::dropHandler
<div class="classTable">
    <div class="classLayout">
        <div class="classBlock">
            <div class="classRow"><div class="classSect">dropHandler()</div></div>
            <div class="classRow"><div class="classDesc">
            When a handler is finished processing events on a stream. It can call dropHandler() to remove itself from the event loop.<br>
            This is usually done automatically by higher level derived handlers.
            </div></div>
        </div>
    </div>
</div>
#### HandlerBase::addHandler
<div class="classTable">
    <div class="classLayout">
        <div class="classBlock">
            <div class="classRow"><div class="classSect">addHandler(Args&&... args)</div></div>
            <div class="classRow"><div class="classDesc">
            If the handler wants to create other handlers.<br>
            An example of this is when the SQL handlers are added. They create connections to the SQL server that require their own event handlers.
            </div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classSect">Parameters</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classItem">args</div><div class="classValue">Arguments passed to the constructor of the new handler</div></div>
        </div>
    </div>
</div>
#### HandlerBase::moveHandler
<div class="classTable">
    <div class="classLayout">
        <div class="classBlock">
            <div class="classRow"><div class="classSect">moveHandler(Args&&... args)</div></div>
            <div class="classRow"><div class="classDesc">
            This is similar to `addHandler()`.<br>
            The difference is that the current handler will immediately suspend until the created handler complets.<br>
            When the added handler calles `dropHandler()` control will be returned to the current handler at the point it suspended.<br>
            <br>
            Note: The current handler must be suspendable (otherwise an exception is thrown).
            See:  <a href="HandlerBase::suspendable">HandlerBase::suspendable</a>
            </div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classSect">Parameters</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classItem">args</div><div class="classValue">Arguments passed to the constructor of the new handler</div></div>
        </div>
    </div>
</div>
#### HandlerBase::eventActivate
<div class="classTable">
    <div class="classLayout">
        <div class="classBlock">
            <div class="classRow"><div class="classSect">eventActivate(LibSocketId sockId, short eventType)</div></div>
            <div class="classRow"><div class="classDesc">
            Method called when there is data on the socket.<br>
            </div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classSect">Return Value</div></div>
            <div class="classRow"><div class="classDesc">
            Return the type of event that you can next process on this stream: EV_READ or EV_WRITE or (EV_READ | EV_WITE)
            </div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classSect">Parameters</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classItem">sockId</div><div class="classValue">The socket with data available on it.</div></div>
            <div class="classRow"><div class="classItem">eventType</div><div class="classValue">The type of event that caused this handler to be triggered (EV_READ | EV_WRITE)</div></div>
        </div>
    </div>
</div>
#### HandlerBase::suspendable
<div class="classTable">
    <div class="classLayout">
        <div class="classBlock">
            <div class="classRow"><div class="classSect">suspendable()</div></div>
            <div class="classRow"><div class="classDesc">
            Return true if the handler is suspendable and false otherwise.<br>
            It is best to inherit from `HandlerNonSuspendable` or aHandlerNonSuspendable` rather than implement this yourself.
            </div></div>
        </div>
    </div>
</div>
#### HandlerBase::suspend
<div class="classTable">
    <div class="classLayout">
        <div class="classBlock">
            <div class="classRow"><div class="classSect">suspend(short type)</div></div>
            <div class="classRow"><div class="classDesc">
            Suspend the execution of this class until `type` (EV_READ | EV_WRITE) is available for this socket.<br>
            This basically returns control to the main event loop until data is available. This is used by the stream handlers to return control to the main event loop if they would block when reading from a stream.
            </div></div>
        </div>
    </div>
</div>
#### HandlerBase::close
<div class="classTable">
    <div class="classLayout">
        <div class="classBlock">
            <div class="classRow"><div class="classSect">close()</div></div>
            <div class="classRow"><div class="classDesc">
            Close the stream associated with this handler.
            </div></div>
        </div>
    </div>
</div>
### HandlerStream

<div class="classTable allowFloat">
    <div class="classLayout">
        <div class="classBlock">
            <div class="classRow"><div class="classItem">Header:   </div><div class="classValue">ThorsNisseCoreService/Handler.h</div></div>
            <div class="classRow"><div class="classItem">NameSpace:</div><div class="classValue">ThorsAnvil::Nisse::Core::Service</div></div>
            <div class="classRow"><div class="classItem">Class:    </div><div class="classValue">HandlerStream</div></div>
            <div class="classRow"><div class="classItem">Parent:   </div><div class="classValue">HandlerBase</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classDesc">
            This class is templatized based on the type of stream the socket represents.<br>
            The class basically defines a common class for holding the stream object and how to close it when requried.
            </div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classSect">Public Members:</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="methodRet"></div><div class="methodName">HandlerStream(Server& parent, LibSocketId socketId, short eventType)</div><div class="methodMark"></div></div>
            <div class="classRow"><div class="methodRet">void</div><div class="methodName">close()</div><div class="methodMark"></div></div>
        </div>
    </div>
</div>
### HandlerNonSuspendable

<div class="classTable allowFloat">
    <div class="classLayout">
        <div class="classBlock">
            <div class="classRow"><div class="classItem">Header:   </div><div class="classValue">ThorsNisseCoreService/Handler.h</div></div>
            <div class="classRow"><div class="classItem">NameSpace:</div><div class="classValue">ThorsAnvil::Nisse::Core::Service</div></div>
            <div class="classRow"><div class="classItem">Class:    </div><div class="classValue">HandlerNonSuspendable</div></div>
            <div class="classRow"><div class="classItem">Parent:   </div><div class="classValue">HandlerStream</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classDesc">
            Defines `suspendable()` and `suspend()` for a class that is non suspendable.<br>
            This is used by some of ther server built in handlers that must complete. It is unlikely that this will be useful for a user defined handler.
            </div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classSect">Public Members:</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="methodRet"></div><div class="methodName">HandlerNonSuspendable(Server& parent, LibSocketId socketId, short eventType)</div><div class="methodMark"></div></div>
            <div class="classRow"><div class="methodRet"></div>bool<div class="methodName">suspendable()</div><div class="methodMark"></div></div>
            <div class="classRow"><div class="methodRet"></div>void<div class="methodName">suspend(short type)</div><div class="methodMark"></div></div>
        </div>
    </div>
</div>
### HandlerSuspendable

<div class="classTable allowFloat">
    <div class="classLayout">
        <div class="classBlock">
            <div class="classRow"><div class="classItem">Header:   </div><div class="classValue">ThorsNisseCoreService/Handler.h</div></div>
            <div class="classRow"><div class="classItem">NameSpace:</div><div class="classValue">ThorsAnvil::Nisse::Core::Service</div></div>
            <div class="classRow"><div class="classItem">Class:    </div><div class="classValue">HandlerSuspendable</div></div>
            <div class="classRow"><div class="classItem">Parent:   </div><div class="classValue">HandlerStream</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classDesc">
            Defines a handler that is suspendable.<br>
            Implements `suspendable`, `suspsend()` and `eventActivate()` as these all work together to define a class that can be suspended.<br>
            The method `eventActivateNonBlocking()` should be overwridden by derived classes to provide functionaliy.
            </div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classSect">Public Members:</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="methodRet"></div><div class="methodName">HandlerSuspendable(Server& parent, LibSocketId socketId, short eventType)</div><div class="methodMark"></div></div>
            <div class="classRow"><div class="methodRet"></div>bool<div class="methodName">suspendable()</div><div class="methodMark"></div></div>
            <div class="classRow"><div class="methodRet"></div>void<div class="methodName">suspend(short type)</div><div class="methodMark"></div></div>
            <div class="classRow"><div class="methodRet"></div>short<div class="methodName">eventActivate(LibSocketId sockId, short eventType)</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classSect">Virtual Members:</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="methodRet">bool</div><div class="methodName">eventActivateNonBlocking()</div><div class="methodMark"></div></div>
        </div>
    </div>
</div>
#### HandlerSuspendable::eventActivate
<div class="classTable">
    <div class="classLayout">
        <div class="classBlock">
            <div class="classRow"><div class="classSect">eventActivate(LibSocketId sockId, short eventType)</div></div>
            <div class="classRow"><div class="classDesc">
            On first call will start the method `eventActivateNonBlocking()`. This method may suspend itself by calling `suspend()`.<br>
            On subsequent calls will resume `eventActivateNonBlocking()` at the point where `suspend()` was called.<br>
            If `eventActivateNonBlocking()` returns true then `dropHandler()` is called to remove this handler as processing is complete.<br>
            </div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classSect">Return Value</div></div>
            <div class="classRow"><div class="classDesc">
            This method returns the type of socket event that should be listend for in the main event loop.
            </div></div>
        </div>
    </div>
</div>
#### HandlerBase::eventActivateNonBlocking
<div class="classTable">
    <div class="classLayout">
        <div class="classBlock">
            <div class="classRow"><div class="classSect">eventActivateNonBlocking()</div></div>
            <div class="classRow"><div class="classDesc">
            Called by eventActivate() to start processing of the stream.<br>
            If a blocking operation is about to be performed this method should call `suspend()` to return control to the main event loop.
            </div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classSect">Return Value</div></div>
            Return true to cause the handler to be re-used.<br>
            Return false to not drop the handler. This is used if something clever is happening.
            <div class="classRow"><div class="classDesc">
            </div></div>
        </div>
    </div>
</div>
### HandlerSuspendableWithStream

<div class="classTable allowFloat">
    <div class="classLayout">
        <div class="classBlock">
            <div class="classRow"><div class="classItem">Header:   </div><div class="classValue">ThorsNisseCoreService/Handler.h</div></div>
            <div class="classRow"><div class="classItem">NameSpace:</div><div class="classValue">ThorsAnvil::Nisse::Core::Service</div></div>
            <div class="classRow"><div class="classItem">Class:    </div><div class="classValue">HandlerSuspendableWithStream</div></div>
            <div class="classRow"><div class="classItem">Parent:   </div><div class="classValue">HandlerSuspendable</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classDesc">
            An implementation of `eventActivateNonBlocking()` that creates input and output stream objects.<br>
            These stream objects will call `suspend()` if they are about to perform a blocking operation on the underlying socket.<br>
            <br>
            Thus we have transparently non-blocking streams.
            </div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classSect">Public Members:</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="methodRet"></div><div class="methodName">HandlerSuspendable(Server& parent, LibSocketId socketId, short eventType)</div><div class="methodMark"></div></div>
            <div class="classRow"><div class="methodRet">bool</div><div class="methodName">eventActivateNonBlocking()</div><div class="methodMark"></div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classSect">Virtual Members:</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="methodRet">bool</div><div class="methodName">eventActivateWithStream(std::istream& input, std::ostream& output)</div><div class="methodMark"></div></div>
        </div>
    </div>
</div>
### ServerHandler

<div class="classTable allowFloat">
    <div class="classLayout">
        <div class="classBlock">
            <div class="classRow"><div class="classItem">Header:   </div><div class="classValue">ThorsNisseCoreService/ServerHandlers.h</div></div>
            <div class="classRow"><div class="classItem">NameSpace:</div><div class="classValue">ThorsAnvil::Nisse::Core::Service</div></div>
            <div class="classRow"><div class="classItem">Class:    </div><div class="classValue">ServerHandler</div></div>
            <div class="classRow"><div class="classItem">Parent:   </div><div class="classValue">HandlerNonSuspendable</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classDesc">
            An implementation of HandlerNonSuspendable that is used to accept connections and create other handlers.
            </div></div>
        </div>
    </div>
</div>
### TimerHandler

<div class="classTable allowFloat">
    <div class="classLayout">
        <div class="classBlock">
            <div class="classRow"><div class="classItem">Header:   </div><div class="classValue">ThorsNisseCoreService/ServerHandlers.h</div></div>
            <div class="classRow"><div class="classItem">NameSpace:</div><div class="classValue">ThorsAnvil::Nisse::Core::Service</div></div>
            <div class="classRow"><div class="classItem">Class:    </div><div class="classValue">TimerHandler</div></div>
            <div class="classRow"><div class="classItem">Parent:   </div><div class="classValue">HandlerNonSuspendable</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classDesc">
            An implementation of HandlerNonSuspendable that is used to implement the timer functionality.
            </div></div>
        </div>
    </div>
</div>


