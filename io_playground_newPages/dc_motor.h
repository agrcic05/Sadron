const char dc_motor_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 Motor Control</title>
  <style>
    :root {
      --bg-color: #f0f2f5;
      --container-bg: #fff;
      --text-color: #333;
      --secondary-text-color: #666;
      --accent-color: #4CAF50;
      --button-bg-color: #3498db; /* Default button color */
      --button-hover-bg-color: #2980b9; /* Hover effect color */
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

    /* Toggle Button Styles */
    .toggle-switch {
      position: relative;
      width: 60px;
      height: 30px;
      display: inline-block;
      margin: 20px 0;
    }

    .toggle-switch input {
      opacity: 0;
      width: 0;
      height: 0;
    }

    .slider {
      position: absolute;
      top: 0;
      left: 0;
      right: 0;
      bottom: 0;
      background-color: #ccc;
      border-radius: 30px;
      cursor: pointer;
      transition: 0.4s;
    }

    .slider:before {
      position: absolute;
      content: "";
      height: 22px;
      width: 22px;
      left: 4px;
      bottom: 4px;
      background-color: white;
      border-radius: 50%;
      transition: 0.4s;
      box-shadow: 0px 2px 5px rgba(0, 0, 0, 0.2);
    }

    input:checked + .slider {
      background-color: var(--accent-color);
    }

    input:checked + .slider:before {
      transform: translateX(30px);
    }

    /* Motor Status Indicator */
    #status {
      font-size: 18px;
      font-weight: bold;
      color: var(--text-color);
      margin-top: 20px;
      opacity: 0;
      transition: opacity 0.4s ease;
    }

    /* Home Button */
    #homeButton {
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

    #homeButton:hover {
      background-color: var(--button-hover-bg-color);
    }

    /* Loading Indicator */
    #loading {
      font-size: 20px;
      color: var(--text-color);
      position: fixed;
      top: 10px;
      left: 50%;
      transform: translateX(-50%);
      display: none;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>DC Motor Control</h1>
    <p>Use the switch below to turn the motor on or off.</p>

    <!-- Toggle Switch -->
    <label class="toggle-switch">
      <input type="checkbox" id="motorToggle" onclick="toggleMotor()">
      <span class="slider"></span>
    </label>

    <!-- Motor Status Display -->
    <div id="status">Motor is OFF</div>
    
    <!-- Home Button -->
    <button id="homeButton" onclick="goHome()">Home</button>
  </div>

  <div id="loading">Connecting...</div>

  <script>
    // WebSocket to communicate with ESP32
    const socket = new WebSocket(`ws://${window.location.hostname}:81/`);

    socket.onopen = () => {
      document.getElementById("loading").style.display = "none";
    };

    socket.onclose = () => {
      document.getElementById("loading").style.display = "block";
      document.getElementById("loading").innerText = "Disconnected. Reconnecting...";
    };

    // Function to toggle motor and send command
    function toggleMotor() {
      const motorStatus = document.getElementById("motorToggle").checked;
      socket.send("toggleMotor");
      document.getElementById("status").style.opacity = 0;
      setTimeout(() => {
        document.getElementById("status").innerText = motorStatus ? "Motor is ON" : "Motor is OFF";
        document.getElementById("status").style.opacity = 1;
      }, 400);
    }

    // Update status based on WebSocket message
    socket.onmessage = function(event) {
      const message = event.data;
      if (message === "ON") {
        document.getElementById("motorToggle").checked = true;
        document.getElementById("status").innerText = "Motor is ON";
      } else if (message === "OFF") {
        document.getElementById("motorToggle").checked = false;
        document.getElementById("status").innerText = "Motor is OFF";
      }
      document.getElementById("status").style.opacity = 1;
    };

    // Home button function
    function goHome() {
      window.location.href = "/"; // Replace with the actual home page URL if different
    }
  </script>
</body>
</html>
)rawliteral";