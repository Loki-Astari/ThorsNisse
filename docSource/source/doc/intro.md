---
layout: plain
generate: false
---

[ThorsNisse](https://github.com/Loki-Astari/ThorsNisse) is RESTful server framework for C++.  
It is based on Node.js and its family of associated packages. ThorsNisse uses transparently non-blocking `std::streams` to achieve high throughput and low complexity of development on a single threaded server.

What does **transparently non-blocking:** mean?

* To the user of the std::stream it will appear to be standard blocking stream; thus allowing very normal looking C++ code to be written.
* To make the framework efficient any blocking read/write calls transfers control back to the Nisse Framework so that other streams may be processed.

