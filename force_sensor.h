const char force_sensor_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Force Sensor</title>
  <style>
    body { font-family: Arial, sans-serif; text-align: center; }
    h1 { font-size: 24px; }
    #forceDisplay { font-size: 18px; margin-top: 20px; }
  </style>
</head>
<body>
  <h1>Force Sensor</h1>
  <div id="forceDisplay">
    <p>Force Sensor Data:</p>
    <p>Analog Reading: <span id="fsrReading">--</span></p>
    <p>Voltage: <span id="voltage">--</span> V</p>
    <p>Force: <span id="force">--</span> N</p>
  </div>
  <button onclick="goToHome()">Home</button>
  <script>
    var socket;

    function initWebSocket() {
      socket = new WebSocket('ws://' + window.location.hostname + ':81/');
      socket.onmessage = function(event) {
        var data = event.data.split(",");
        document.getElementById('fsrReading').innerText = data[0];
        document.getElementById('voltage').innerText = data[1];
        document.getElementById('force').innerText = data[2];
      };

      socket.onerror = function(event) {
        console.error("WebSocket error:", event);
      };

      refreshForceData();
    }

    function refreshForceData() {
      if (socket.readyState === WebSocket.OPEN) {
        socket.send('refreshForce');
      } else {
        console.error("WebSocket connection is not open.");
      }
    }

    function goToHome() {
      window.location.href = "/";
    }

    initWebSocket();
    setInterval(refreshForceData, 2000);
  </script>
</body>
</html>
)rawliteral";