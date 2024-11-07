const char servo_control_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
  <head>
    <title>Servo Control</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      body { font-family: Arial, sans-serif; text-align: center; }
      input[type="range"] {
        width: 300px;
      }
      h1 { color: #333; }
      a { text-decoration: none; font-size: 20px; color: #007BFF; }
    </style>
  </head>
  <body>
    <h1>Servo Motor Control</h1>
    <p>Move the slider to set the servo position (0-180 degrees).</p>
    <input type="range" min="0" max="180" value="90" id="servoSlider">
    <p>Servo Position: <span id="servoValue">90</span>°</p>
    
    <br>
    <a href="/">Back to Home</a>  <!-- Link to the homepage -->

    <script>
      var slider = document.getElementById("servoSlider");
      var servoValue = document.getElementById("servoValue");
      var lastSentPosition = 90;
      var debounceTimeout;

      // Establish WebSocket connection
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

        // Debounce slider input to prevent sending too many updates too fast
        clearTimeout(debounceTimeout);
        debounceTimeout = setTimeout(function() {
          sendServoPosition(slider.value);
        }, 100);  // 100 ms debounce time
      };

      function sendServoPosition(position) {
        if (position != lastSentPosition) {
          if (ws.readyState === WebSocket.OPEN) {
            ws.send('servoPosition:' + position);
            lastSentPosition = position;  // Update last sent position
            console.log("Sent servo position: " + position);
          } else {
            console.log("WebSocket not open, cannot send message.");
          }
        }
      }
    </script>
  </body>
</html>
)rawliteral";