---
layout: article
generate: false
---

```cpp
// Must include this header
#include "ThorsExpress/all.h"

namespace HTTP = ThorsAnvil::Nisse::Protocol::HTTP;

// Must define this function once.
void addSite(HTTP::Site& site)
{
    site.get("/listBeer", [](HTTP::Request& request, HTTP::Response& response)
    {
    });
    // You can add as many resources to as site as you need.
}
```

Express is a simple library that allows the definition of simple HTTP site modules that can be loaded/unload dynamically. Though each site object is simple each one can be bound to a different relative paths on a site.


