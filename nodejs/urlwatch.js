#!/usr/bin/env node
/*
 *
 *
 */
 


var Blink1 = require('node-blink1');
var http = require('http');

var testmode = 0;
var intervalSecs = 2;

var requrl = 'http://todbot.com/tst/color.txt';

// parse commandline args
process.argv.forEach(function (val, index, array) {
        if( val.indexOf("http") == 0 ) { 
            requrl = val;
        }
        else if( val.indexOf("test") == 0 ) { 
            testmode = 1;
        }
});

console.log("watching every "+intervalSecs+" the url: "+requrl);

// open a blink1 for use
var blink1;
try { 
  blink1 = new Blink1.Blink1();
} catch(err) {
  console.log("no blink1 devices found");
  if( !testmode ) process.exit(1);
}


reqcallback = function(response) {
  var str = ''
  response.on('data', function (chunk) {
          str += chunk;
  });

  response.on('end', function () {
          //console.log(":"+str+":");
          var regex = /([0-9A-F]{2})([0-9A-F]{2})([0-9A-F]{2})/;
          var result = str.match(regex);
          if( result ) {
              var red = parseInt( result[1],16 );
              var grn = parseInt( result[2],16 );
              var blu = parseInt( result[3],16 );
              console.log("found rgb: " + red +","+ grn +","+ blu +"");
              if( blink1 == null ) return;
              try {  
                  blink1.fadeToRGB( 100, red,grn,blu );
              } catch(err) { 
                  console.log(err);  // might get this if your USB port is weird/flaky
              }
          }
  });
};

timercallback = function() { 
    var req = http.request(requrl, reqcallback);
    req.end();
};

timercallback();
setInterval( timercallback, intervalSecs * 1000 );

