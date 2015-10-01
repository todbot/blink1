Node.js API for blink(1)
========================

## node-blink1, officially-supported blink1 library for Node:
Authored by @sandeepmistry, supported by ThingM
- https://npmjs.org/package/node-blink1

### To use:
Install locally into your project (for Node 0.12)
```
% npm install node-blink1
```

### About node-blink1 and Node 4.x
As of 30 Sep 2015, node-hid has been patched to work with Node 4 by @lorenc-tomasz,
but the hasn't been merged with mainline node-hid.

If you want to use node-blink1 with Node 4.x,
install this fork of node-hid before node-blink1:
```
% npm install 'https://github.com/lorenc-tomasz/node-hid.git#package'
% npm install node-blink1
```

### Examples of node-blink1 in use:
- node-blink1-server - HTTP REST API server in Node for blink(1) devices
  - https://www.npmjs.com/package/node-blink1-server
- node-red-node-blink1 - Node-RED node to control a blink(1)
  - https://www.npmjs.com/package/node-red-node-blink1
- meshblu-blink1 - Meshblu blink(1) connector for use in Octoblu or with other services
  - https://www.npmjs.com/package/meshblu-blink1
- buildblink - monitor continuous integration builds and notify you via blink(1)
  - https://www.npmjs.com/package/buildblink
- grunt-blink1 - Configures blink(1) inside your Gruntfile
  - https://www.npmjs.com/package/grunt-blink1  
- tweet-blink - real-time visualtion of tweets using blink(1)
  - https://www.npmjs.com/package/tweet-blink



