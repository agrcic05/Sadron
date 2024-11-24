const char led_control_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>LED Control</title>
  <style>
    :root {
      --bg-color: linear-gradient(135deg, #C8102E, #54585A); /* Scarlet and Grey gradient */
      --container-bg: #fff;
      --text-color: #333;
      --button-bg-color: #000; /* Black button background */
      --button-hover-bg-color: #444; /* Darker black for hover */
      --button-text-color: #fff;
      --button-border-radius: 5px;
      --button-padding: 8px 12px;
      --button-font-size: 14px;
      --led-size: 40px;
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
      width: 90%;
      max-width: 600px;
    }

    h1 {
      font-size: 24px;
      margin-bottom: 20px;
    }

    .led-container {
      display: flex;
      justify-content: space-around;
      align-items: center;
      margin-bottom: 20px;
    }

    .led {
      width: var(--led-size);
      height: var(--led-size);
      background-color: gray;
      border-radius: 50%;
      box-shadow: inset 0px 0px 10px rgba(0, 0, 0, 0.5);
      display: inline-block;
      margin: 5px;
      transition: background-color 0.3s ease;
    }

    .led.on-red {
      background-color: red;
    }

    .led.on-yellow {
      background-color: yellow;
    }

    .led.on-green {
      background-color: green;
    }

    .button {
      padding: var(--button-padding);
      font-size: var(--button-font-size);
      color: var(--button-text-color);
      background-color: var(--button-bg-color);
      border: none;
      border-radius: var(--button-border-radius);
      cursor: pointer;
      transition: background-color 0.3s ease;
      margin: 5px;
    }

    .button:hover {
      background-color: var(--button-hover-bg-color);
    }

    .voltage-display {
      margin-top: 5px;
      font-size: 12px;
      color: #333;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>LED Control</h1>
    <div class="led-container">
      <div>
        <div id="led1" class="led"></div>
        <button class="button" onclick="toggleLED(1)">LED 1</button>
        <div id="voltage1" class="voltage-display">0 V</div>
      </div>
      <div>
        <div id="led2" class="led"></div>
        <button class="button" onclick="toggleLED(2)">LED 2</button>
        <div id="voltage2" class="voltage-display">0 V</div>
      </div>
      <div>
        <div id="led3" class="led"></div>
        <button class="button" onclick="toggleLED(3)">LED 3</button>
        <div id="voltage3" class="voltage-display">0 V</div>
      </div>
      <div>
        <div id="led4" class="led"></div>
        <button class="button" onclick="toggleLED(4)">LED 4</button>
        <div id="voltage4" class="voltage-display">0 V</div>
      </div>
      <div>
        <div id="led5" class="led"></div>
        <button class="button" onclick="toggleLED(5)">LED 5</button>
        <div id="voltage5" class="voltage-display">0 V</div>
      </div>
      <div>
        <div id="led6" class="led"></div>
        <button class="button" onclick="toggleLED(6)">LED 6</button>
        <div id="voltage6" class="voltage-display">0 V</div>
      </div>
    </div>
    <button class="button" onclick="goToHome()">Home</button>
  </div>

  <script>
    const ledColors = {
      1: 'red',
      2: 'red',
      3: 'yellow',
      4: 'yellow',
      5: 'green',
      6: 'green'
    };

    function toggleLED(ledNumber) {
      var ws = new WebSocket('ws://' + window.location.hostname + ':81/');
      ws.onopen = function() {
        ws.send('toggleLED' + ledNumber);
      };
      ws.onmessage = function(event) {
        updateLEDIndicator(ledNumber, event.data);
        ws.close();
      };
    }

    function goToHome() {
      window.location.href = "/";
    }

    function updateLEDIndicator(ledNumber, state) {
      var ledElement = document.getElementById("led" + ledNumber);
      var voltageElement = document.getElementById("voltage" + ledNumber);
      
      ledElement.className = 'led';
      if (state === 'ON') {
        ledElement.classList.add(`on-${ledColors[ledNumber]}`);
        voltageElement.textContent = '5 V';
      } else {
        voltageElement.textContent = '0 V';
      }
    }

    function fetchInitialLEDStates() {
      var ws = new WebSocket('ws://' + window.location.hostname + ':81/');
      ws.onopen = function () {
        ws.send('getLEDStates');
      };
      ws.onmessage = function (event) {
        try {
          const states = JSON.parse(event.data); // Parse JSON data
          updateAllLEDIndicators(states);
        } catch (error) {
          console.error("Error parsing LED states:", error);
        }
        ws.close();
      };
    }

    function updateAllLEDIndicators(states) {
      for (let i = 1; i <= 6; i++) {
        updateLEDIndicator(i, states[i] === 'ON' ? 'ON' : 'OFF'); // Check state for ON/OFF
      }
    }

    window.onload = fetchInitialLEDStates;
  </script>
</body>
</html>
)rawliteral";
