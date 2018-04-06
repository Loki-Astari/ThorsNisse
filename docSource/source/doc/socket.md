---
layout: article
generate: false
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

A simple wrapper around BSD sockets so they are easy to use in C++.
<dl>
<dt>NameSpace:</dt><dd>ThorsAnvil::Nisse::Core::Socket</dd>
<dt>Headers:</dt><dd>ThorsNisseCoreSocket/</dd>
<dt>Socket.h</dt><dd>

* class BaseSocket
* class DataSocket: public BaseSocket
* class ConnectionSocket: public DataSocket
* class ServerSocket: public BaseSocket

</dd>
<dt>SocketStream.h</dt><dd>

* class SocketStreamBuffer: public std::streambuf
* class ISocketStream: public std::istream
* class OSocketStream: public std::ostream

</dd>
</dl>

### BaseSocket

<div class="classTable">
    <div class="classLayout">
        <div class="classBlock">
            <div class="classRow"><div class="classItem">Header:   </div><div class="classValue">ThorsNisseCoreSocket/Socket.h</div></div>
            <div class="classRow"><div class="classItem">NameSpace:</div><div class="classValue">ThorsAnvil::Nisse::Core::Socket</div></div>
            <div class="classRow"><div class="classItem">Class:    </div><div class="classValue">BaseSocket</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classDesc">Base of all the socket classes.<br>
            This class should not be directly created.<br>
            All socket classes are movable but not copyable.</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classSect">Public Members:</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="methodRet"></div><div class="methodName">BaseSocket(BaseSocket&&)</div><div class="methodMark">noexcept</div></div>
            <div class="classRow"><div class="methodRet">BaseSocket&</div><div class="methodName">operator=(BaseSocket&&)</div><div class="methodMark">noexcept</div></div>
            <div class="classRow"><div class="methodRet">void</div><div class="methodName">swap(BaseSocket&)</div><div class="methodMark">noexcept</div></div>
            <div class="classRow"><div class="methodRet">void</div><div class="methodName">close()</div><div class="methodMark"></div></div>
        </div>
    </div>
</div>
### DataSocket

<div class="classTable">
    <div class="classLayout">
        <div class="classBlock">
            <div class="classRow"><div class="classItem">Header:   </div><div class="classValue">ThorsNisseCoreSocket/Socket.h</div></div>
            <div class="classRow"><div class="classItem">NameSpace:</div><div class="classValue">ThorsAnvil::Nisse::Core::Socket</div></div>
            <div class="classRow"><div class="classItem">Class:    </div><div class="classValue">DataSocket</div></div>
            <div class="classRow"><div class="classItem">Parent:   </div><div class="classValue">BaseSocket</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classDesc">
            Data sockets define the read/write interface to a socket.<br>
            This class should not be directly created
            </div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classSect">Public Members:</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="methodRet">std::pair&lt;bool, std::size_t&gt;</div><div class="methodName">getMessageData(char* buffer, std::size_t size, std::size_t alreadyGot = 0)</div><div class="methodMark"></div></div>
            <div class="classRow"><div class="methodRet">std::pair&lt;bool, std::size_t&gt;</div><div class="methodName">putMessageData(char const* buffer, std::size_t size, std::size_t alreadyPut = 0)</div><div class="methodMark"></div></div>
            <div class="classRow"><div class="methodRet">void</div><div class="methodName">putMessageClose()</div><div class="methodMark"></div></div>
        </div>
    </div>
</div>

#### DataSocket::getMessageData

<div class="classTable">
    <div class="classLayout">
        <div class="classBlock">
            <div class="classRow"><div class="classSect">getMessageData()</div></div>
            <div class="classRow"><div class="classDesc">
            Reads data from a sokcet into a buffer.<br>
            If the stream is blocking will not return until the requested amount of data has been read or there is no more data to read.<br>
            If the stream in non blocking will return if the read operation would block.
            </div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classSect">Return Value</div></div>
            <div class="classRow"><div class="classDesc">
            This method returns a pair: <code>std::pair&lt;bool, std::size_t&gt;</code><br>
            The first member `bool` indicates if more data can potentially be read from the stream. If the socket was cut or the EOF reached then this value will be false<br>
            The second member `std::size_t` indicates exactly how many bytes were read from this stream.
            </div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classSect">Parameters</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classItem">buffer</div><div class="classValue">The buffer data will be read into.</div></div>
            <div class="classRow"><div class="classItem">size</div><div class="classValue">The size of the buffer.</div></div>
            <div class="classRow"><div class="classItem">alreadyGot</div><div class="classValue">Offset into buffer (and amount size is reduced by) as this amount was read on a previous call).</div></div>
        </div>
    </div>
</div>
#### DataSocket::putMessageData

<div class="classTable">
    <div class="classLayout">
        <div class="classBlock">
            <div class="classRow"><div class="classSect">putMessageData()</div></div>
            <div class="classRow"><div class="classDesc">
            Writes data from a buffer to a sokcet.<br>
            If the stream is blocking will not return until the requested amount of data has been written or the socket was closed to writting.<br>
            If the stream in non blocking will return if the write operation would block.
            </div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classSect">Return Value</div></div>
            <div class="classRow"><div class="classDesc">
            This method returns a pair: <code>std::pair&lt;bool, std::size_t&gt;</code><br>
            The first member `bool` indicates if more data can potentially be written to the stream. If the socket was cut or closed then this value will be false<br>
            The second member `std::size_t` indicates exactly how many bytes were written to this stream.
            </div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classSect">Parameters</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classItem">buffer</div><div class="classValue">The buffer data will be written from.</div></div>
            <div class="classRow"><div class="classItem">size</div><div class="classValue">The size of the buffer.</div></div>
            <div class="classRow"><div class="classItem">alreadyPut</div><div class="classValue">Offset into buffer (and amount size is reduced by) as this amount was written on a previous call).</div></div>
        </div>
    </div>
</div>
#### DataSocket::putMessageClose

<div class="classTable">
    <div class="classLayout">
        <div class="classBlock">
            <div class="classRow"><div class="classSect">putMessageClose()</div></div>
            <div class="classRow"><div class="classDesc">
            closes the write end of the socket and flushes (write) data.
            </div></div>
        </div>
    </div>
</div>
### ConnectionSocket

<div class="classTable">
    <div class="classLayout">
        <div class="classBlock">
            <div class="classRow"><div class="classItem">Header:   </div><div class="classValue">ThorsNisseCoreSocket/Socket.h</div></div>
            <div class="classRow"><div class="classItem">NameSpace:</div><div class="classValue">ThorsAnvil::Nisse::Core::Socket</div></div>
            <div class="classRow"><div class="classItem">Class:    </div><div class="classValue">ConnectionSocket</div></div>
            <div class="classRow"><div class="classItem">Parent:   </div><div class="classValue">DataSocket</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classDesc">
            Creates a connection to <code>host</code> on <code>port</code>.<br>
            Note this class inherits from <code>DataSocket</code> so once created you can read/write to the socket.
            </div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classSect">Public Members:</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="methodRet"></div><div class="methodName">ConnectionSocket(std::string const& host, int port)</div><div class="methodMark"></div></div>
        </div>
    </div>
</div>

### ServerSocket

<div class="classTable">
    <div class="classLayout">
        <div class="classBlock">
            <div class="classRow"><div class="classItem">Header:   </div><div class="classValue">ThorsNisseCoreSocket/Socket.h</div></div>
            <div class="classRow"><div class="classItem">NameSpace:</div><div class="classValue">ThorsAnvil::Nisse::Core::Socket</div></div>
            <div class="classRow"><div class="classItem">Class:    </div><div class="classValue">ServerSocket</div></div>
            <div class="classRow"><div class="classItem">Parent:   </div><div class="classValue">BaseSocket</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classDesc">
            </div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classSect">Public Members:</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="methodRet"></div><div class="methodName">ServerSocket(int port, bool blocking, int maxWaitConnections = ServerSocket::maxConnectionBacklog)</div><div class="methodMark"></div></div>
            <div class="classRow"><div class="methodRet">DataSocket</div><div class="methodName">accept(bool blocking)</div><div class="methodMark"></div></div>
        </div>
    </div>
</div>
#### ServerSocket::accept

<div class="classTable">
    <div class="classLayout">
        <div class="classBlock">
            <div class="classRow"><div class="classSect">accept()</div></div>
            <div class="classRow"><div class="classDesc">
            "Accept" a waiting connection request on the port and creates a two way socket for communication on another port.<br>
            If this is a non blocking socket and there is no waiting connection an exception is thrown.<br>
            If this is a blocking socket wait for a connection<br>
            </div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classSect">Return Value</div></div>
            <div class="classRow"><div class="classDesc">
            A <code>DataSocket</code> is returned so data can be exchange across the socket.
            </div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classSect">Parameters</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classItem">blocking</div><div class="classValue">Passed to the constructor of the <code>DataSocket</code> that is returned.</div></div>
        </div>
    </div>
</div>
### SocketStreamBuffer

<div class="classTable">
    <div class="classLayout">
        <div class="classBlock">
            <div class="classRow"><div class="classItem">Header:   </div><div class="classValue">ThorsNisseCoreSocket/SocketStream.h</div></div>
            <div class="classRow"><div class="classItem">NameSpace:</div><div class="classValue">ThorsAnvil::Nisse::Core::Socket</div></div>
            <div class="classRow"><div class="classItem">Class:    </div><div class="classValue">SocketStreamBuffer</div></div>
            <div class="classRow"><div class="classItem">Parent:   </div><div class="classValue">std::streambug</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classDesc">
            This is a wrapper class for a <code>DataSocket</code> that allows the socket to be treated like <code>std::streambuf</code>.<br>
            This class overrides just enough virtual functions to make the <code>ISocketStream</code> and <code>OSocketStream</code> useful.<br>
            This class provides no public API and is designed to be used solely with the following stream objects.
            </div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classSect">Public Members:</div></div>
        </div>
    </div>
</div>
### ISocketStream

<div class="classTable">
    <div class="classLayout">
        <div class="classBlock">
            <div class="classRow"><div class="classItem">Header:   </div><div class="classValue">ThorsNisseCoreSocket/SocketStream.h</div></div>
            <div class="classRow"><div class="classItem">NameSpace:</div><div class="classValue">ThorsAnvil::Nisse::Core::Socket</div></div>
            <div class="classRow"><div class="classItem">Class:    </div><div class="classValue">ISocketStream</div></div>
            <div class="classRow"><div class="classItem">Parent:   </div><div class="classValue">std::istream</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classDesc">
            An implementation of <code>std::istream</code> that uses <code>SocketStreamBuffer</code> as the buffer.<br>
            The <code>Notofer</code> is a primitive event callback mechanism.<br>
            A blocking read call to these streams calls the <code>Notifier noData</code>. This is used by the <a href="#Server">Server</a> infastructure to yield control back to the main event loop.<br>
            <code>using Notifier = std::function<void()>;</code>
            </div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classSect">Public Members:</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="methodRet"></div><div class="methodName">ISocketStream(DataSocket& stream, Notifier noData, Notfier flush)</div><div class="methodMark"></div></div>
            <div class="classRow"><div class="methodRet"></div><div class="methodName">ISocketStream(DataSocket& stream)</div><div class="methodMark"></div></div>
            <div class="classRow"><div class="methodRet"></div><div class="methodName">ISocketStream(ISocketStream&& move)</div><div class="methodMark"></div></div>
        </div>
    </div>
</div>
### OSocketStream

<div class="classTable">
    <div class="classLayout">
        <div class="classBlock">
            <div class="classRow"><div class="classItem">Header:   </div><div class="classValue">ThorsNisseCoreSocket/SocketStream.h</div></div>
            <div class="classRow"><div class="classItem">NameSpace:</div><div class="classValue">ThorsAnvil::Nisse::Core::Socket</div></div>
            <div class="classRow"><div class="classItem">Class:    </div><div class="classValue">OSocketStream</div></div>
            <div class="classRow"><div class="classItem">Parent:   </div><div class="classValue">std::istream</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classDesc">
            An implementation of <code>std::ostream</code> that uses <code>SocketStreamBuffer</code> as the buffer.<br>
            The <code>Notofer</code> is a primitive event callback mechanism.
            A blocking write call to these streams calls the <code>Notifier noData</code>. This is used by the <a href="#Server">Server</a> infastructure to yield control back to the main event loop.<br>
            <code>using Notifier = std::function<void()>;</code>
            </div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="classSect">Public Members:</div></div>
        </div>
        <div class="classBlock">
            <div class="classRow"><div class="methodRet"></div><div class="methodName">OSocketStream(DataSocket& stream, Notifier noData, Notfier flush)</div><div class="methodMark"></div></div>
            <div class="classRow"><div class="methodRet"></div><div class="methodName">OSocketStream(DataSocket& stream)</div><div class="methodMark"></div></div>
            <div class="classRow"><div class="methodRet"></div><div class="methodName">OSocketStream(OSocketStream&& move)</div><div class="methodMark"></div></div>
        </div>
    </div>
</div>

