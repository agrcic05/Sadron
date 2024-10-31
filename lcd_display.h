const char lcd_display_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>LCD Display Control</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
</head>
<body>
  <h2>LCD Display Control</h2>
  <input type="text" id="lcdInput" placeholder="Enter text" maxlength="16">
  <button onclick="sendLCDText()">Send to LCD</button>
  <br><br>
  <a href="/">Back to Home</a>
  <script>
    function sendLCDText() {
      var lcdText = document.getElementById('lcdInput').value;
      var ws = new WebSocket('ws://' + window.location.hostname + ':81');
      ws.onopen = function() {
        ws.send('lcd:' + lcdText);
        ws.close();
      };
    }
  </script>
</body>
</html>
)rawliteral";