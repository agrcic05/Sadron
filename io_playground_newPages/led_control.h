const char led_control_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>LED Control</title>
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
      --led-size: 60px;
      --container-padding: 20px;
      --container-border-radius: 10px;
      --container-box-shadow: 0px 4px 8px rgba(0, 0, 0, 0.2);
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
      text-align: center;
    }

    .container {
      background-color: var(--container-bg);
      padding: var(--container-padding);
      border-radius: var(--container-border-radius);
      box-shadow: var(--container-box-shadow);
      width: 80%;
      max-width: 400px;
    }

    h1 {
      font-size: 24px;
      margin-bottom: 20px;
    }

    /* LED Indicator */
    .led {
      width: var(--led-size);
      height: var(--led-size);
      background-color: gray;
      border-radius: 50%;
      box-shadow: inset 0px 0px 10px rgba(0, 0, 0, 0.3);
      display: inline-block;
      transition: background-color 0.3s ease;
      margin-bottom: 20px;
    }
    
    .led.on {
      background-color: #f00;
    }

    /* Button Styling */
    .button {
      display: block;
      width: 100%;
      margin-top: 10px;
      padding: var(--button-padding);
      font-size: var(--button-font-size);
      color: var(--button-text-color);
      background-color: var(--button-bg-color);
      border: none;
      border-radius: var(--button-border-radius);
      cursor: pointer;
      transition: background-color 0.3s ease;
    }

    .button:hover {
      background-color: var(--button-hover-bg-color);
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>LED Control</h1>
    <div class="led" id="ledIndicator"></div>
    <button class="button" onclick="toggleLED()">Toggle LED</button>
    <button class="button" onclick="goToHome()">Home</button>
  </div>

  <script>
    function toggleLED() {
      var ws = new WebSocket('ws://' + window.location.hostname + ':81/');
      ws.onopen = function(event) {
        ws.send('toggleLED');
      };
      ws.onmessage = function(event) {
        updateLEDIndicator(event.data);
        ws.close();
      };
    }

    function goToHome() {
      window.location.href = "/";
    }

    // Update LED indicator based on initial LED state
    window.onload = function() {
      var ws = new WebSocket('ws://' + window.location.hostname + ':81/');
      ws.onopen = function(event) {
        ws.send('getLEDState');
      };
      ws.onmessage = function(event) {
        updateLEDIndicator(event.data);
        ws.close();
      };
    };

    function updateLEDIndicator(state) {
      var ledIndicator = document.getElementById('ledIndicator');
      if (state === 'ON') {
        ledIndicator.classList.add('on');
      } else {
        ledIndicator.classList.remove('on');
      }
    }
  </script>
</body>
</html>
)rawliteral";