
## blink1-tiny-server

A simple HTTP to blink(1) gateway

- Uses blink1-lib to talk to blink(1)
- Uses Mongoose : https://github.com/cesanta/mongoose which is included

To build:
```
cd blink1/commandline && make blink1-tiny-server
```

Usage:
```
usage:
  ./commandline/blink1-tiny-server [options]
where options are:
  -p <port> -- port to start server on

Supported URIs:
    /blink1/on  -- turn blink1 on full white
    /blink1/off -- turn blink1 off
    /blink1/red -- turn blink1 red #FF0000
    /blink1/green -- turn blink1 green #00FF00
    /blink1/blue  -- turn blin1 blue #0000FF
    /blink1/fadeToRGB?rgb=%23ff00ff&time=1.0  -- fade to a color over a time
    /blink1/blink?rgb=%23ff0ff&time=1.0&count=3 -- blink a color, with time & repeats
```
