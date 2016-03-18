blink1buildlight
================

About this package
------------------

A python zmq build-light controller for the Blink(1).

This package (blink1buildlight) is designed to control multiple Blink(1) devices in a corporate development environment. It assumes:
* At least one, but possibly a very large number of Blink(1) lights.
* At least one but possibly a large number of systems generating events.

This package uses pyzeromq for networking. It uses the new Blink(1) control library which requires pyusb.

This package is designed to work on Python 3.x.

Installation
------------

    pip install blink1buildlight
    
Some additional hardware configuration may be required - please refer to the Blink(1)'s documentation.

Use
---

Set up a build-light proxy
--------------------------

On a single computer run the **b1_proxy** script. This is the coordinator for the build-light network. It will randomly
assign itself two TCP ports and begin advertising:

    bl_proxy
   
Set up a build-light controller
-------------------------------
   
Launch as many build-light controllers as you want. Build-light controllers must have at least one Blink(1) attached.
 
    bl_controller
    
By default the script assumes that your ambient lighting is similar to a halogen bulb, however it supports any white-point
correction known to the blink1 library.

    bl_controller --white_point=tungsten
    
This script will attempt to detect a proxy on the network. This can take a few seconds depending on your proxy's configuration.
This script is responsible for directly controlling the build light.

When the controller starts up it automatically enters 'throb' mode. This displays a rainbow sequence of colours signaling
that it's ready to receive instructions from the proxy.

Set the throb state
-------------------

"Throbbing" is the default state of the build-light. A throb will contunue until a flash event occurs. The throb will resume
once the flash has completed.

The command-line tool **bl_set_throb** can be used to set the throb parameters for all of the controllers on the network:

    bl_set_throb --url tcp://halob:61638 --freq=0.25 --colours=navy,cyan,blue
    
If the url argument is omitted the tool will attempt to detect a proxy on the network. 

Set the flash state
-------------------

"Flashing" can be used to signal an event, the precise meaning of a flash is to be determined by the users.

The command-line tool **bl_set_flash** can be used to make the build-lights flash:

    bl_set_flash --url tcp://halob:61638 --colour=white --duration=3
    
When the flash is complete the build-lights will return to 'throb' state.

Build light control API
-----------------------

We provide a simple to use context-manager that connects to the bl_proxy. 

    def set_throb(url=None, freq=1.0, colours="red,green,blue"):
        col_list = [c.strip() for c in colours.split(',')]
        with get_build_light_proxy(context, url=url) as blr:
            blr.throb(freq=freq, colours=col_list)
            
    def set_flash(url=None, colour="white", times=3, duration=1):
        with get_build_light_proxy(context, url=url) as blr:
            blr.flash(
                colour=colour,
                duration=duration,
                times=times
            )