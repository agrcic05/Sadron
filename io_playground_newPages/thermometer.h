const char thermometer_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Thermometer</title>
  <style>
    :root {
      --bg-color: #f0f2f5;
      --container-bg: #fff;
      --text-color: #333;
      --secondary-text-color: #666;
      --accent-color: #4CAF50;
      --button-bg-color: #3498db;
      --button-hover-bg-color: #2980b9;
      --button-text-color: #fff;
      --button-border-radius: 5px;
      --button-padding: 12px 20px;
      --button-font-size: 16px;
    }

    body {
      background: linear-gradient(135deg, #74EBD5, #ACB6E5);
      display: flex;
      justify-content: center;
      align-items: center;
      height: 100vh;
      margin: 0;
      font-family: Arial, sans-serif;
      color: var(--text-color);
    }

    .container {
      text-align: center;
      background-color: var(--container-bg);
      padding: 20px;
      border-radius: 10px;
      box-shadow: 0px 4px 8px rgba(0, 0, 0, 0.2);
      width: 80%;
      max-width: 400px;
    }

    h1 {
      font-size: 24px;
      margin-bottom: 20px;
    }

    h2 {
      font-size: 20px;
      margin-top: 20px;
    }

    /* Thermometer Styling */
    #thermometer {
      width: 50px;
      height: 300px;
      position: relative;
      margin: auto;
      border-radius: 25px;
      background: linear-gradient(to top, #0000FF, #FF0000);
      box-shadow: inset 0px 4px 10px rgba(0, 0, 0, 0.2);
    }

    #mercury {
      width: 100%;
      position: absolute;
      bottom: 0;
      background-color: #ff0000;
      border-radius: 25px 25px 0 0;
      transition: height 0.5s;
    }

    /* Home Button */
    #homeButton {
      margin-top: 20px;
      padding: var(--button-padding);
      font-size: var(--button-font-size);
      color: var(--button-text-color);
      background-color: var(--button-bg-color);
      border: none;
      border-radius: var(--button-border-radius);
      cursor: pointer;
      transition: background-color 0.3s ease;
    }

    #homeButton:hover {
      background-color: var(--button-hover-bg-color);
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>Thermometer</h1>
    <div id="thermometer">
      <div id="mercury"></div>
    </div>
    <h2>Temperature: <span id="temp">--</span>°C</h2>
    <button id="homeButton" onclick="goToHome()">Home</button>
  </div>

  <script>
    var socket;

    function initWebSocket() {
      socket = new WebSocket('ws://' + window.location.hostname + ':81/');
      
      socket.onmessage = function(event) {
        var temperature = parseFloat(event.data);
        updateThermometer(temperature);
      };
      
      socket.onerror = function(event) {
        console.error("WebSocket error:", event);
      };
      
      refreshTemperature();
    }

    function refreshTemperature() {
      if (socket.readyState === WebSocket.OPEN) {
        socket.send('refreshTemp');
      } else {
        console.error("WebSocket connection is not open.");
      }
    }

    function updateThermometer(temp) {
      var minValue = -5;
      var maxValue = 50;
      var mercury = document.getElementById('mercury');
      var height = 300;
      var maxHeight = height - 20;
      var minHeight = 10;
      var range = maxValue - minValue;

      temp = Math.min(Math.max(temp, minValue), maxValue);
      var mercuryHeight = Math.min(Math.max((temp - minValue) / range * maxHeight, minHeight), maxHeight);
      mercury.style.height = mercuryHeight + 'px';
      document.getElementById('temp').innerText = temp.toFixed(2) + '°C';

      var percent = (temp - minValue) / (maxValue - minValue);
      var color = interpolateColor("#0000FF", "#FF0000", percent);
      document.getElementById('thermometer').style.background = 'linear-gradient(to top, #0000FF, ' + color + ')';
    }

    function interpolateColor(startColor, endColor, percent) {
      var startRGB = hexToRgb(startColor);
      var endRGB = hexToRgb(endColor);

      var r = Math.ceil(startRGB.r + percent * (endRGB.r - startRGB.r));
      var g = Math.ceil(startRGB.g + percent * (endRGB.g - startRGB.g));
      var b = Math.ceil(startRGB.b + percent * (endRGB.b - startRGB.b));

      return rgbToHex(r, g, b);
    }

    function hexToRgb(hex) {
      var bigint = parseInt(hex.substring(1), 16);
      var r = (bigint >> 16) & 255;
      var g = (bigint >> 8) & 255;
      var b = bigint & 255;
      return { r: r, g: g, b: b };
    }

    function rgbToHex(r, g, b) {
      return "#" + ((1 << 24) + (r << 16) + (g << 8) + b).toString(16).slice(1);
    }

    function goToHome() {
      window.location.href = "/";
    }

    initWebSocket();
    setInterval(refreshTemperature, 2000);
  </script>
</body>
</html>
)rawliteral";