---
layout: package
generate: false
nameSpace:  ThorsAnvil::Nisse::Core::Utility
headers:
    base: ThorsNisseCoreUtility/
    files:
        - name:   utility.h
          classes: []
          methods:
            - return: std::string,
              name:   systemErrorMessage
              param:  []
            - return: std::string
              name:   buildErrorMessage
              param:  []
---

```cpp
if (open("file", O_WRONLY) == -1)
{
    throw std::runtime_error(
        buildErrorMessage("MyClass::", __func__,
                          ": open: ", Utility::systemErrorMessage()));
}
```
```cpp--DeepDive
// Utility.h
/*
 * Builds a string for a system error message.
 * uses `errno` to build the name of the error and the associated message into a string.
 */
inline std::string systemErrorMessage();

/*
 * Build an error message from a set of parameters.
 * Slightly more compact than using 'operator<<` very useful for building exception messages.
 */
template<typename... Args>
std::string buildErrorMessage(Args const&... args);
```

Provides common utility functions for other packages.

