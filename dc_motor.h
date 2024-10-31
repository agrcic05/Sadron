const char dc_motor_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>DC Motor Control</title>
  <style>
    body { font-family: Arial, sans-serif; text-align: center; }
    h1 { font-size: 24px; }
    button { font-size: 16px; padding: 10px 20px; }
  </style>
</head>
<body>
  <h1>DC Motor Control</h1>
  <button id="toggleMotor" onclick="toggleMotor()">Turn On</button><br>
  <h2>Status: <span id="motorStatus">Off</span></h2>
  <button onclick="goToHome()">Home</button>
  <script>
    var socket = new WebSocket('ws://' + window.location.hostname + ':81/');

    socket.onmessage = function(event) {
      if (event.data === 'ON') {
        document.getElementById('motorStatus').innerText = 'On';
        document.getElementById('toggleMotor').innerText = 'Turn Off';
      } else {
        document.getElementById('motorStatus').innerText = 'Off';
        document.getElementById('toggleMotor').innerText = 'Turn On';
      }
    };

    function toggleMotor() {
      socket.send('toggleMotor');
    }

    function goToHome() {
      window.location.href = "/";
    }
  </script>
</body>
</html>
)rawliteral";