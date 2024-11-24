const char dc_motor_temp_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>DC Motor & Temperature</title>
  <style>
    :root {
      --bg-color: #C8102E; /* Scarlet */
      --secondary-bg-color: #54585A; /* Grey */
      --container-bg: #fff; /* White */
      --text-color: #000; /* Black */
      --button-bg-color: #000;
      --button-hover-bg-color: #444;
      --button-text-color: #fff;
      --button-border-radius: 5px;
      --button-padding: 12px 20px;
      --button-font-size: 16px;
    }

    body {
      background: linear-gradient(135deg, var(--bg-color), var(--secondary-bg-color));
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
      width: 90%;
      max-width: 500px;
    }

    h1 {
      font-size: 24px;
      margin-bottom: 20px;
    }

    h2, h3 {
      margin: 15px 0;
    }

    /* Thermometer Styling */
    #thermometer {
      width: 50px;
      height: 300px;
      position: relative;
      margin: 20px auto;
      border-radius: 25px;
      background: linear-gradient(to top, #0000FF, #FF0000);
      box-shadow: inset 0px 4px 10px rgba(0, 0, 0, 0.2);
    }

    #mercury {
      width: 100%;
      position: absolute;
      bottom: 0;
      background-color: #FF0000;
      border-radius: 25px 25px 0 0;
      transition: height 0.5s;
    }

    /* Button Styling */
    button {
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

    button:hover {
      background-color: var(--button-hover-bg-color);
    }

    /* Select Dropdown */
    select {
      padding: 10px;
      font-size: 16px;
      border-radius: 5px;
      border: 1px solid #ccc;
      outline: none;
      background-color: #fff;
      color: #000;
      margin: 10px 0;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>DC Motor & Temperature Status</h1>
    <div id="thermometer">
      <div id="mercury"></div>
    </div>
    <h2>Temperature: <span id="temp-celsius">--</span>°C | <span id="temp-fahrenheit">--</span>°F</h2>
    <h2>Motor Status: <span id="motor-status">--</span></h2>

    <h3>Set Temperature Threshold (20°C to 35°C):</h3>
    <select id="temperatureThreshold" onchange="setTemperatureThreshold()">
      <option value="20">20°C</option>
      <option value="21">21°C</option>
      <option value="22">22°C</option>
      <option value="23">23°C</option>
      <option value="24">24°C</option>
      <option value="25">25°C</option>
      <option value="26">26°C</option>
      <option value="27">27°C</option>
      <option value="28">28°C</option>
      <option value="29">29°C</option>
      <option value="30" selected>30°C</option>
      <option value="31">31°C</option>
      <option value="32">32°C</option>
      <option value="33">33°C</option>
      <option value="34">34°C</option>
      <option value="35">35°C</option>
    </select>
    <button onclick="goToHome()">Home</button>
  </div>

  <script>
    var socket;

    function initWebSocket() {
        socket = new WebSocket('ws://' + window.location.hostname + ':81/');
        socket.onmessage = function(event) {
            var data = JSON.parse(event.data);
            updateThermometer(data.temperature);
            updateMotorStatus(data.motorOn);
        };
        
        socket.onerror = function(event) {
            console.error("WebSocket error:", event);
        };

        refreshData();
    }
        function refreshData() {
        if (socket.readyState === WebSocket.OPEN) {
            socket.send('refreshData');
        } else {
            console.error("WebSocket connection is not open.");
        }
    }

    function setTemperatureThreshold() {
      var threshold = document.getElementById("temperatureThreshold").value;
      if (socket.readyState === WebSocket.OPEN) {
        socket.send('setThreshold:' + threshold);
      }
    }

    function updateThermometer(temp) {
  var mercury = document.getElementById('mercury');
  var percent = Math.min(Math.max((temp + 5) / 55, 0), 1); // Normalize temp to percentage
  mercury.style.height = (percent * 100) + '%';

  document.getElementById('temp-celsius').innerText = temp.toFixed(2);
  document.getElementById('temp-fahrenheit').innerText = (temp * 9 / 5 + 32).toFixed(2);

  // Adjust thermometer gradient based on temperature
  var color = interpolateColor("#0000FF", "#FF0000", percent); // Blue (cold) to Red (hot)
  document.getElementById('thermometer').style.background = `linear-gradient(to top, #0000FF, ${color})`;
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

    function updateMotorStatus(isMotorOn) {
      document.getElementById('motor-status').innerText = isMotorOn ? "ON" : "OFF";
    }

    function goToHome() {
      window.location.href = "/";
    }

    initWebSocket();
    setInterval(refreshData, 2000); // Send `refreshData` every 1 seconds
  </script>
</body>
</html>
)rawliteral";
