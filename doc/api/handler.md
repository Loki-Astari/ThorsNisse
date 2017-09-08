[![Build Status](https://travis-ci.org/Loki-Astari/ThorsNisse.svg?branch=master)](https://travis-ci.org/Loki-Astari/ThorsNisse)

![ThorStream](../../img/Nisse.jpg)

A handler object is created for every connected port. There are three main classes

* [HandlerBase](#HandlerBase)  
The base class of all the handlers. You should probably not derive from this class.
* [HandlerNonSuspendable](#HandlerNonSuspendable)  
A handler that can not be suspended when another connection is made in the code. If you create other handlers dynamically they will be queued up an handled like normal by the main even loop.
* [HandlerSuspendable](#HandlerSuspendable)  
A handler that can be suspended. If you create another dynamic handler this handler will be suspended until the other handler completes. When it completes this handler will resume. This is used by the HTTP handler to create SQL connections to a server. The SQL connection/action must complete before this server can continue, therefore this handler will be suspended until the SQL connection has completed. This happens completely transparently to the user code.

<div id="#HandlerBase"/>
### class HandlerBase
* virtual short eventActivate(LibSocketId sockId, short eventType) = 0;  
This is the main callback entry point when there is data available on the socket.

sockId:    The socket number data is available.
eventType: The type of data available EV_READ or EV_WRITE or EV_READ | EV_WRITE

The value returned should be EV_READ or EV_WRITE or EV_READ | EV_WRITE depending on what type of information you want to processes next. You can return 0 to indicate that you don't want to listen for more events. This is not recommended unless you have another handler that will re-start the listeners.

* virtual bool  suspendable() = 0;  
Return true if this handler can be suspended on a call to `moveHandler()`. If this function return false `moveHandler()` will behave like `addHandler()`.

* virtual void suspend(short) = 0;  
Called when the handler needs to be suspended. The handler should yields processing back to the main event loop.

* virtual void  close() = 0;  
Called when the handler shuts down to force all buffered data to be flushed.

<div id="#HandlerNonSuspendable"/>
### template<typename Stream> class HandlerNonSuspendable: public HandlerStream<Stream>
Stream is usually `Sokcet::DataStream`

* virtual bool suspendable()  final;  
Returns false
* virtual void suspend(short) final;  
Throws, this is not suspendable.
* virtual void  close();  
Calls `stream.close()`

<div id="#HandlerSuspendable"/>
### class HandlerSuspendable: public HandlerStream<Stream>
Stream is usually `Sokcet::DataStream`

This classes uses boost::co_routine to provide a suspendable/resumable functionality.

* virtual bool suspendable()  final;  
Returns true
* virtual void suspend(short) final;  
Calls yield() on the boost::co_routine.
* virtual void  close();  
Calls `stream.close()`
* virtual short eventActivate(LibSocketId sockId, short eventType) final;  
This uses boost::co_routines to handle re-entry. It calls `eventActivateNonBlocking()` on the first call. Subsequent calls will ask the boost::co_routine to restart the function from the yield point.
* virtual bool eventActivateNonBlocking() = 0;  
This function returns control to the main event loop by calling `suspend()`. When it completes it returns true to indicate that the handler object should be destroyed and false if the handler object should not be destroyed.


