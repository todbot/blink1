chrome.app.runtime.onLaunched.addListener(function() {
  chrome.app.window.create('blink1test1.html', {
    bounds: {
      width: 400,
      height: 400
    }
  });
});
