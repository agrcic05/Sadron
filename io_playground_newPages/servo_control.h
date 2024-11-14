const char servo_control_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Servo Motor Control</title>
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
      background-color: var(--bg-color);
      color: var(--text-color);
      transition: background-color 0.4s ease, color 0.4s ease;
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

    p {
      color: var(--secondary-text-color);
      font-size: 16px;
      margin: 10px 0;
    }

    /* Range Slider */
    input[type="range"] {
      -webkit-appearance: none;
      width: 100%;
      margin: 20px 0;
      height: 6px;
      background: var(--accent-color);
      border-radius: 5px;
      outline: none;
    }
    
    input[type="range"]::-webkit-slider-thumb {
      -webkit-appearance: none;
      appearance: none;
      width: 20px;
      height: 20px;
      background-color: #3498db;
      border-radius: 50%;
      cursor: pointer;
      box-shadow: 0px 2px 4px rgba(0, 0, 0, 0.2);
      transition: background-color 0.3s ease;
    }
    
    input[type="range"]::-webkit-slider-thumb:hover {
      background-color: #2980b9;
    }

    /* Back Button */
    #backButton {
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

    #backButton:hover {
      background-color: var(--button-hover-bg-color);
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>Servo Motor Control</h1>
    <p>Move the slider to set the servo position (0-180 degrees).</p>

    <!-- Servo Slider -->
    <input type="range" min="0" max="180" value="90" id="servoSlider">
    <p>Servo Position: <span id="servoValue">90</span>°</p>

    <!-- Back Button -->
    <button id="backButton" onclick="goHome()">Back to Home</button>
  </div>

  <script>
    var slider = document.getElementById("servoSlider");
    var servoValue = document.getElementById("servoValue");
    var lastSentPosition = 90;
    var debounceTimeout;

    // WebSocket connection
    var ws = new WebSocket('ws://' + window.location.hostname + ':81/');

    ws.onopen = function() {
      console.log("WebSocket connected");
    };

    ws.onclose = function() {
      console.log("WebSocket disconnected");
    };

    ws.onmessage = function(event) {
      console.log("Received from server: " + event.data);
    };

    slider.oninput = function() {
      servoValue.innerHTML = this.value;
      clearTimeout(debounceTimeout);
      debounceTimeout = setTimeout(function() {
        sendServoPosition(slider.value);
      }, 100); // debounce for smoother control
    };

    function sendServoPosition(position) {
      if (position != lastSentPosition) {
        if (ws.readyState === WebSocket.OPEN) {
          ws.send('servoPosition:' + position);
          lastSentPosition = position;
          console.log("Sent servo position: " + position);
        } else {
          console.log("WebSocket not open, cannot send message.");
        }
      }
    }

    function goHome() {
      window.location.href = "/";
    }
  </script>
</body>
</html>
)rawliteral";