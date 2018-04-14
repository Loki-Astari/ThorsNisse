---
layout: method
generate: false
methodInfo:
    parent: Server
    name: addTimer
    mark:  
    description:  |
        Sets a timer to go off every `timeOut` seconds.
        The result of the timmer going off is to execute the functot `action`.
    return:
        type: ' void'
        description: ''
    parameters:
        - name: timeOut
          type: 'double'
          default: 
          description: 'The time period (in seconds) between running the action object. Functor that is run every `timeOut` seconds.'
        - name: action
          type: 'std::function<void()>&&'
          default: 
          description: 'The time period (in seconds) between running the action object. Functor that is run every `timeOut` seconds.'
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

