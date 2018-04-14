---
layout: method
generate: false
methodInfo:
    parent: Server
    name: addTimer
    description: Sets a timer to go off every `timeOut` seconds.<br>The result of the timmer going off is to execute the functot `action`.
    parameters:
        - name: timeOut
          type: double
          description: The time period (in seconds)  between running the action object.
        - name: action
          type: std::function<void()>&&
          description: Functor that is run every `timeOut` seconds.
---
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

