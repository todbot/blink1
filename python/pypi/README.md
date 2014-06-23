Python3 Blink(1) library
========================

About this library
------------------

This is a rewrite of ThingM's original Python library. It includes the following modifications:

* 100% test coverage on all library components
* Python 3.x compatible
* Automatic installation via Python Package Index.
* Higher level control over the blink(1)git s

Installation
------------

    pip install blink1
    
Use
---

The simplest way to use this library is via a context manager.

    import time
    from blink1.blink1 import blink1
    
    with blink1() as b1:
        b1.fade_to_color(100, 'navy')
        time.sleep(10)
        
When the blink1() block exits the light is automatically switched off.


