const char dc_motor_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>DC Motor Control</title>
  <style>
    :root {
      --bg-gradient: linear-gradient(135deg, #C8102E, #54585A); /* Scarlet and Grey Gradient */
      --button-bg-color: #000; /* Button background color */
      --button-hover-bg-color: #444; /* Button hover color */
      --button-text-color: #fff; /* Button text color */
      --button-border-radius: 5px;
      --button-padding: 12px 20px;
      --button-font-size: 16px;
      --text-color: #000; /* Text color */
      --secondary-text-color: #ddd; /* Secondary text color */
    }

    body {
      background: var(--bg-gradient);
      display: flex;
      justify-content: center;
      align-items: center;
      height: 100vh;
      margin: 0;
      font-family: Arial, sans-serif;
      color: var(--text-color);
      transition: background-color 0.4s ease, color 0.4s ease;
    }

    .container {
      text-align: center;
      background-color: #fff; /* White background for container */
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
      margin-top: 10px;
    }

    .switch {
      position: relative;
      display: inline-block;
      width: 60px;
      height: 34px;
    }

    .switch input {
      opacity: 0;
      width: 0;
      height: 0;
    }

    .slider {
      position: absolute;
      cursor: pointer;
      top: 0;
      left: 0;
      right: 0;
      bottom: 0;
      background-color: #ccc;
      transition: 0.4s;
      border-radius: 34px;
    }

    .slider:before {
      position: absolute;
      content: "";
      height: 26px;
      width: 26px;
      left: 4px;
      bottom: 4px;
      background-color: white;
      transition: 0.4s;
      border-radius: 50%;
    }

    input:checked + .slider {
      background-color: #4CAF50;
    }

    input:checked + .slider:before {
      transform: translateX(26px);
    }

    /* Button styles */
    button {
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
    <h1>DC Motor Control</h1>
    <label class="switch">
      <input type="checkbox" id="motorToggle" onchange="debounceToggleMotor()">
      <span class="slider"></span>
    </label>
    <h2>Status: <span id="motorStatus">Off</span></h2>
    <h2>Voltage: <span id="motorVoltage">0V</span></h2>
    <button onclick="goToHome()">Home</button>
  </div>

<script>
  let debounceTimer;

  function debounceToggleMotor() {
    if (debounceTimer) return;
    debounceTimer = setTimeout(() => debounceTimer = null, 200);

    toggleMotor();
  }

  var socket = new WebSocket('ws://' + window.location.hostname + ':81/');
  
  // Handle messages from server to update UI
  socket.onmessage = function(event) {
    if (event.data === 'ON') {
      document.getElementById('motorStatus').innerText = 'On';
      document.getElementById('motorVoltage').innerText = '5V'; // Update voltage to 5V when motor is ON
      document.getElementById('motorToggle').checked = true;
    } else if (event.data === 'OFF') {
      document.getElementById('motorStatus').innerText = 'Off';
      document.getElementById('motorVoltage').innerText = '0V'; // Update voltage to 0V when motor is OFF
      document.getElementById('motorToggle').checked = false;
    }
  };

  // Function to send toggle request only
  function toggleMotor() {
    if (socket.readyState === WebSocket.OPEN) {
      socket.send('toggleMotor');
    } else {
      console.error("WebSocket connection is not open.");
    }
  }

  // Function to handle initial motor check when the page loads
  socket.onopen = function() {
    checkMotorStatus(); // Check the motor's current status on page load
  };

  // Function to check motor status
  function checkMotorStatus() {
    if (socket.readyState === WebSocket.OPEN) {
      socket.send('checkMotorStatus'); // Request motor status from the server
    } else {
      console.error("WebSocket connection is not open.");
    }
  }

  // Navigate back to home
  function goToHome() {
    window.location.href = "/";
  }
</script>
</body>
</html>
)rawliteral";
