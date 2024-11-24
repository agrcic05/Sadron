const char pressure_led_control_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Pressure Sensor LED Control</title>
  <style>
    :root {
      --bg-color-gradient: linear-gradient(135deg, #C8102E, #54585A);
      --text-color: #000000;
      --button-bg-color: #000000;
      --button-hover-bg-color: #444444;
      --button-text-color: #ffffff;
      --button-border-radius: 5px;
      --button-padding: 12px 20px;
      --button-font-size: 16px;
      --led-off-color: #555555;
      --led-red-color: #ff0000;
      --led-yellow-color: #ffff00;
      --led-green-color: #00ff00;
      --container-bg-color: #ffffff;
    }

    body {
      display: flex;
      justify-content: center;
      align-items: center;
      height: 100vh;
      margin: 0;
      font-family: Arial, sans-serif;
      background: var(--bg-color-gradient);
      color: var(--text-color);
    }

    .container {
      text-align: center;
      padding: 20px;
      border-radius: 10px;
      box-shadow: 0px 4px 8px rgba(0, 0, 0, 0.2);
      background-color: var(--container-bg-color);
      width: 90%;
      max-width: 400px;
    }

    h1 {
      font-size: 24px;
      margin-bottom: 20px;
    }

    h2 {
      font-size: 20px;
      margin: 10px 0;
    }

    .led-indicator {
      display: flex;
      justify-content: center;
      gap: 10px;
      margin: 20px 0;
    }

    .led {
      width: 25px;
      height: 25px;
      border-radius: 50%;
      background-color: var(--led-off-color);
    }

    .led.red.on {
      background-color: var(--led-red-color);
    }

    .led.yellow.on {
      background-color: var(--led-yellow-color);
    }

    .led.green.on {
      background-color: var(--led-green-color);
    }

    button {
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

    button:hover {
      background-color: var(--button-hover-bg-color);
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>Pressure Sensor LED Control</h1>
    <p>Apply pressure to see the LEDs turn on!</p>
    <div class="led-indicator" id="leds">
      <div class="led red" id="led1"></div>
      <div class="led red" id="led2"></div>
      <div class="led yellow" id="led3"></div>
      <div class="led yellow" id="led4"></div>
      <div class="led green" id="led5"></div>
      <div class="led green" id="led6"></div>
    </div>
    <h2 id="forceDisplay">Force: 0N</h2>
    <h2 id="voltageDisplay">Voltage: 0V</h2>
    <button onclick="goHome()">Home</button>
  </div>

  <script>
    const voltageThresholds = [0.10, 0.15, 0.25, 0.35, 0.50, 0.70];
    const ws = new WebSocket('ws://' + location.hostname + ':81/');

    function initWebSocket() {
      ws.onmessage = function(event) {
        const data = JSON.parse(event.data);

        // Update force and voltage readings
        document.getElementById("forceDisplay").innerText = `Force: ${data.force.toFixed(2)} N`;
        document.getElementById("voltageDisplay").innerText = `Voltage: ${data.voltage.toFixed(2)} V`;

        // Update LED statuses
        voltageThresholds.forEach((threshold, index) => {
          const led = document.getElementById(`led${index + 1}`);
          if (data.voltage >= threshold) {
            led.classList.add("on");
          } else {
            led.classList.remove("on");
          }
        });
      };

      ws.onerror = function(event) {
        console.error("WebSocket error:", event);
      };

      sendPressureLEDRefresh();
    }

    function sendPressureLEDRefresh() {
      if (ws.readyState === WebSocket.OPEN) {
        ws.send('pressureLED');
      } else {
        console.error("WebSocket connection is not open.");
      }
    }

    setInterval(sendPressureLEDRefresh, 500);

    function goHome() {
      window.location.href = "/";
    }

    initWebSocket();
  </script>
</body>
</html>
)rawliteral";
