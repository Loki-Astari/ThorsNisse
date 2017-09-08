## Summary

Application                      | Description
---------------------------------|------------------------------------------------------------------
Node.Cxx                         |

NameSpace                        | Description
---------------------------------|------------------------------------------------------------------
ThorsAnvil/Nisse/Core            |
ThorsAnvil/Nisse/Core/Utility    | Utility functions that are used by other packages.<br>Mostly to build error messages for exceptions.
ThorsAnvil/Nisse/Core/Socket     | C++ wrapper around sockets. Basic sockets and Stream sockets.
ThorsAnvil/Nisse/Core/Service    | The server infastructure built around libevent.
ThorsAnvil/Nisse/Core/SQL        | A non-blocking wrapper around ThorSQL library.
ThorsAnvil/Nisse/Protocol        | 
ThorsAnvil/Nisse/Protocol/Simple | A made up protocol that helps exercises the functionality of Services in the unit tests.
ThorsAnvil/Nisse/Protocol/HTTP   | Classes that make using the HTTP protocol simple in the Node.Cxx server.
ThorsAnvil/Nisse/Express         | A simple wrapper library to make building dynamically loadable server modules easy.

Methods                          | Description
---------------------------------|------------------------------------------------------------------
Utility::buildErrorMessage()     | Builds a single string from multiple arguments. It is used to make build exception messages easy.
Utility::systemErrorMessage()    | Used to build a string for system error messages where `errno` is set by a standard call.<br>Used in conjunction with buildErrorMessage().

Namespace   | Classes                          | Description
------------|----------------------------------|------------------------------------------------------------------
ThorsAnvil::Core::Socket |
&nbsp;| BaseSocket                |
&nbsp;| DataSocket                |
&nbsp;| ConnectSocket             |
&nbsp;| ServerSocket              |
ThorsAnvil::Core::Service |
&nbsp;| ServerConnection          |
&nbsp;| Server                    |
&nbsp;| HandlerBase               |
&nbsp;| HandlerStream&lt;Stream&gt; |
&nbsp;| HandlerNonSuspendable&lt;Stream&gt; |
&nbsp;| HandlerSuspendable&lt;Stream&gt; |
&nbsp;| ServerHandler&lt;ActHand, Param&gt; |
&nbsp;| TimerHandler              |
