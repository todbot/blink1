var BLINK1_VENDOR_ID = 10168;
var BLINK1_PRODUCT_ID = 493;
var DEVICE_INFO = {"vendorId": BLINK1_VENDOR_ID, "productId": BLINK1_PRODUCT_ID};

var blink1Device;
var usb = chrome.usb;

var connected = false;

// this is what a HID Set Feature requests looks like in chrome.usb
var transferInfo = {
    "requestType": "class",
    "recipient": "interface",
    "direction": "out",
    "request":  0x09, // set_feature 
    "value": 1, // report id?
    "index": 0,
    "data": new Uint8Array([0x01,0x63, 0xFF, 0xCC, 0x33, 0x00, 0x20, 0x00,0x00]).buffer
    // note data contains report id as first byte
};

var updateStatus = function(statusStr) {
  blink1Status.innerText = statusStr;
}

var randomColor = function() {
    if( !connected ) return;

    var r = Math.floor((Math.random()*255)+0);
    var g = Math.floor((Math.random()*255)+0);
    var b = Math.floor((Math.random()*255)+0);

    redval.value = r;
    grnval.value = g;
    bluval.value = b;

    console.log("randomColor: "+ r+","+g+","+b);

    blink1_setColor(r,g,b, 100);

    //setTimeout( randomColor, 1000);
}

var blink1_setColor = function(r,g,b, fadeMillis) { 
  var rgbstr = "rgb(" +r+", "+g+", "+b+")";
  console.log("setColor: "+ rgbstr);
  colorswatch.style.backgroundColor = rgbstr;
  if( !connected ) return;  
  var th = (fadeMillis/10) >> 8;
  var tl = (fadeMillis/10) & 0xff;
  transferInfo.data = new Uint8Array([0x01,0x63, r,g,b, th,tl, 0x00,0x00]).buffer
  usb.controlTransfer(blink1Device, transferInfo, onEvent);
}

var updateColor = function() { 
  var r = redval.value
  var g = grnval.value
  var b = bluval.value
  var fadeMillis = 70;
  blink1_setColor( r,g,b, fadeMillis);
}

redval.addEventListener('change', function() {
  updateColor();
});
grnval.addEventListener('change', function() {
  updateColor();
});
bluval.addEventListener('change', function() {
  updateColor();
});

var onEvent=function(usbEvent) {
    if (usbEvent.resultCode) {
      console.log("Error: " + usbEvent.error);
      connected = false;
      return;
    }
  };

var gotPermission = function(result) {
    requestPermissionDiv.style.display = 'none';
    console.log('Appp was granted the "usbDevices" permission.');
    usb.findDevices( DEVICE_INFO, 
      function(devices) {
        if (!devices || !devices.length) {
          updateStatus("no device found");
          console.log('device not found');
          return;
        }
        console.log('Found device: ' + devices[0].handle);
        blink1Device = devices[0];
        console.log("blink1Device:"); console.log(blink1Device);        
        updateStatus("connected!");
        connected = true;
        randomColor();
    });
  };

var permissionObj = {permissions: [{'usbDevices': [DEVICE_INFO] }]};
requestPermission.addEventListener('click', function() {
  chrome.permissions.request( permissionObj, function(result) {
    if (result) {
      gotPermission();
    } else {
      console.log('App was not granted the "usbDevices" permission.');
      console.log(chrome.runtime.lastError);
    }
  });
});

chrome.permissions.contains( permissionObj, function(result) {
  if (result) {
    gotPermission();
  }
});
