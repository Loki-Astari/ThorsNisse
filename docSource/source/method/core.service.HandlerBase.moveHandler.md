---
layout: method
generate: false
methodInfo:
    parent: HandlerBase
    name: moveHandler
    description: This is similar to `addHandler()`.<br>The difference is that the current handler will immediately suspend until the created handler complets.<br>When the added handler calles `dropHandler()` control will be returned to the current handler at the point it suspended.<br><br>Note- The current handler must be suspendable (otherwise an exception is thrown).See-  <a href="HandlerBase::suspendable">HandlerBase::suspendable</a>
    parameters:
        - name: args
          type: ARGS&&...
          description: Arguments passed to the constructor of the new handler
---

