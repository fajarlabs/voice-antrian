
function connectWSEvent(uri, callbackMessage) {
  var ws = new WebSocket(`ws://${window.location.host}/${uri}`)
  
  ws.onopen = function() {
    console.log("Connected");
  };
  
  ws.onmessage = function(e) {
    callbackMessage(e)
  };
  
  ws.onclose = function(e) {
    console.log('Socket is closed. Reconnect will be attempted in 1 second.', e.reason);
    setTimeout(function() {
      connectWSEvent(uri, callbackMessage);
    }, 1000);
  };
}