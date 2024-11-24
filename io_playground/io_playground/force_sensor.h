const char force_sensor_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Force Sensor</title>
  <style>
    :root {
      --bg-color: linear-gradient(135deg, #C8102E, #54585A); /* Scarlet and Grey gradient */
      --container-bg: #fff;
      --text-color: #333;
      --secondary-text-color: #666;
      --accent-color: #4CAF50;
      --button-bg-color: #000; /* Black button background */
      --button-hover-bg-color: #444; /* Darker black for hover */
      --button-text-color: #fff;
      --button-border-radius: 5px;
      --button-padding: 12px 20px;
      --button-font-size: 16px;
      --container-padding: 20px;
      --container-border-radius: 10px;
      --container-box-shadow: 0px 4px 8px rgba(0, 0, 0, 0.2);
    }

    body {
      background: var(--bg-color);
      display: flex;
      justify-content: center;
      align-items: center;
      height: 100vh;
      margin: 0;
      font-family: Arial, sans-serif;
      color: var(--text-color);
      text-align: center;
    }

    .container {
      background-color: var(--container-bg);
      padding: var(--container-padding);
      border-radius: var(--container-border-radius);
      box-shadow: var(--container-box-shadow);
      width: 80%;
      max-width: 400px;
      text-align: center;
    }

    h1 {
      font-size: 24px;
      margin-bottom: 20px;
    }

    #forceDisplay {
      padding: 20px;
      margin: 20px 0;
      background-color: #f9f9f9;
      border-radius: 10px;
      box-shadow: inset 0 4px 8px rgba(0, 0, 0, 0.1);
    }

    #forceDisplay p {
      font-size: 18px;
      color: var(--secondary-text-color);
      margin: 10px 0;
    }

    #forceDisplay span {
      font-weight: bold;
      font-size: 20px;
      color: var(--text-color);
    }

    .button {
      width: 100%;
      padding: var(--button-padding);
      font-size: var(--button-font-size);
      color: var(--button-text-color);
      background-color: var(--button-bg-color);
      border: none;
      border-radius: var(--button-border-radius);
      cursor: pointer;
      transition: background-color 0.3s ease;
      margin-top: 20px;
    }

    .button:hover {
      background-color: var(--button-hover-bg-color);
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>Force Sensor Data</h1>
    
    <div id="forceDisplay">
      <p>Analog Reading: <span id="fsrReading">--</span></p>
      <p>Voltage: <span id="voltage">--</span> V</p>
      <p>Force: <span id="force">--</span> N</p>
    </div>

    <button class="button" onclick="goToHome()">Home</button>
  </div>

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
    setInterval(refreshForceData, 500);
  </script>
</body>
</html>
)rawliteral";