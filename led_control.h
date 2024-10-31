const char led_control_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>LED Control</title>
  <style>
    /* Style for LED indicator */
    .led {
      width: 50px;
      height: 50px;
      background-color: gray;
      border-radius: 50%;
      box-shadow: inset 0px 0px 10px rgba(0, 0, 0, 0.5);
      display: inline-block;
      margin-right: 20px;
    }
    .led.on {
      background-color: #f00; /* Red color for on state */
    }
  </style>
</head>
<body>
  <h1>LED Control</h1>
  <div class="led" id="ledIndicator"></div>
  <button onclick="toggleLED()">Toggle LED</button><br>
  <button onclick="goToHome()">Home</button>
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
      var ledIndicator = document.getElementById('ledIndicator');
      var ws = new WebSocket('ws://' + window.location.hostname + ':81/');
      ws.onopen = function(event) {
        ws.send('getLEDState');
      };
      ws.onmessage = function(event) {
        updateLEDIndicator(event.data);
        ws.close();
      };
    };
    // Function to update LED indicator based on LED state
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
