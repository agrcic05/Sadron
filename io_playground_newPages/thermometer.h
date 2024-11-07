const char thermometer_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Thermometer</title>
  <style>
    body { font-family: "Georgia"; text-align: center; font-size: 16px;}
    h1 { font-weight: bold; font-size: 24px;}
    h2 { font-weight: bold; font-size: 20px;}
    button { font-weight: bold; font-size: 16px;}
    #thermometer {
      width: 200px;
      height: 450px;
     position: relative;
      border-radius: 10px;
      margin: auto;
      background: linear-gradient(to top, #0000FF, #FF0000);
    }
    #mercury {
      width: 100%;
      position: absolute;
      bottom: 0;
      background-color: #ff0000;
      border-radius: 10px 10px 0 0;
      transition: height 0.5s;
    }
  </style>
</head>
<body>
  <h1>Thermometer</h1>
  <div id="thermometer">
    <div id="mercury"></div>
  </div>
  <h2>Temperature: <span id="temp">--</span></h2>
  <button onclick="goToHome()">Home</button>
  <script>
    var socket;

    // Function to initialize WebSocket connection
    function initWebSocket() {
      socket = new WebSocket('ws://' + window.location.hostname + ':81/');

      // WebSocket event listener for receiving temperature data
      socket.onmessage = function(event) {
      var temperature = parseFloat(event.data);
      updateThermometer(temperature);
      };
    
      // WebSocket event listener for handling errors
      socket.onerror = function(event) {
        console.error("WebSocket error:", event);
      };

      // Immediately refresh temperature data upon connection
      refreshTemperature();
    }

    // Function to refresh temperature data
    function refreshTemperature() {
      if (socket.readyState === WebSocket.OPEN) {
        socket.send('refreshTemp');
      } else {
      console.error("WebSocket connection is not open.");
      }
    }

    // Function to update thermometer and temperature display
    function updateThermometer(temp) {
      // Adjust the range to fit temperatures from -5°C to 50°C
      var minValue = -5;
      var maxValue = 50;

      var mercury = document.getElementById('mercury');
      var height = 450; // Maximum height of thermometer
      var maxHeight = height - 20; // Leave some space at the top
      var minHeight = 50; // Minimum height to show
      var range = maxValue - minValue; // Temperature range

      // Adjust the temperature to fit the new range
      temp = Math.min(Math.max(temp, minValue), maxValue);

      var mercuryHeight = Math.min(Math.max((temp - minValue) / range * maxHeight, minHeight), maxHeight);
      mercury.style.height = mercuryHeight + 'px';
      document.getElementById('temp').innerText = temp.toFixed(2) + '°C';

      // Calculate color based on temperature
      var percent = (temp - minValue) / (maxValue - minValue);
      var color = interpolateColor("#0000FF", "#FF0000", percent);
      document.getElementById('thermometer').style.background = 'linear-gradient(to top, #0000FF, ' + color + ')';
    }

    // Interpolate color based on percentage
    function interpolateColor(startColor, endColor, percent) {
      var startRGB = hexToRgb(startColor);
      var endRGB = hexToRgb(endColor);

      var r = Math.ceil(startRGB.r + percent * (endRGB.r - startRGB.r));
      var g = Math.ceil(startRGB.g + percent * (endRGB.g - startRGB.g));
      var b = Math.ceil(startRGB.b + percent * (endRGB.b - startRGB.b));

      return rgbToHex(r, g, b);
    }

    // Convert hex color to RGB
    function hexToRgb(hex) {
      var bigint = parseInt(hex.substring(1), 16);
      var r = (bigint >> 16) & 255;
      var g = (bigint >> 8) & 255;
      var b = bigint & 255;
      return { r: r, g: g, b: b };
    }

    // Convert RGB color to hex
    function rgbToHex(r, g, b) {
      return "#" + ((1 << 24) + (r << 16) + (g << 8) + b).toString(16).slice(1);
    }

    // Function to navigate to home page
    function goToHome() {
      window.location.href = "/";
    }
    
    // Initial WebSocket connection
    initWebSocket();

    // Periodically refresh temperature data every 2 seconds
    setInterval(refreshTemperature, 2000);
  </script>
</body>
</html>
)rawliteral";
