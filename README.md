[![Build Status](https://travis-ci.org/Loki-Astari/ThorsNisse.svg?branch=master)](https://travis-ci.org/Loki-Astari/ThorsNisse)

![ThorStream](img/Nisse.jpg)

# ThorsNisse

This is RESTful server framework for C++.  
It is based on the Node.js server and uses transparently non-blocking streams to achieve high throughput and low complexity of development on a single threaded server.

## Objectives

1. A framework that allows functors (including lambda's) to specify the actions of a RESTful end point.  
When invoked, these functors will behave like normal C++ routines that use blocking streams (thus allowing for very standard C++ coding style); but the streams themselves will behave like non-blocking; a blocking read/write request will transfer control back to Nisse server framework.

1. Allow modules to be loaded/unloaded dynamically from a running server.  
To simplify the development processes a newly built module can be automatically loaded into a running server for immediate testing.

1. Provide non-blocking accesses to common DBs.  
This is provided via the ThorSQL library (but requires some integration with the Nisse Framework).


## Details

* Examples
  * [Building a Server](doc/server.md)
  * [Building a Module](doc/module.md)
* Documentation
  * [Usage](doc/usage.md)
  * [Internal](doc/internal.md)
  * [DB Accesses](doc/dbaccess.md)

