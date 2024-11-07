const char force_sensor_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Force Sensor</title>
  <style>
    /* General styles */
    body {
      font-family: 'Roboto', sans-serif;
      margin: 0;
      padding: 0;
      background: linear-gradient(135deg, #74EBD5, #ACB6E5); /* Gradient background */
      display: flex;
      justify-content: center;
      align-items: center;
      height: 100vh;
    }
    
    .container {
      text-align: center;
      background-color: #fff;
      padding: 20px;
      border-radius: 10px;
      box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
      width: 80%;
      max-width: 600px;
    }

    h1 {
      font-size: 30px;
      color: #333;
      margin-bottom: 20px;
    }

    /* Data display section */
    #forceDisplay {
      padding: 20px;
      margin: 20px 0;
      border-radius: 8px;
      box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
    }

    #forceDisplay p {
      font-size: 18px;
      color: #555;
      margin: 10px 0;
    }

    #forceDisplay span {
      font-weight: bold;
      font-size: 20px;
      color: #333;
    }

    /* Button styling */
    button {
      padding: 12px 24px;
      font-size: 18px;
      background-color: #007BFF;
      color: white;
      border: none;
      border-radius: 5px;
      cursor: pointer;
      margin-top: 20px;
      transition: background-color 0.3s;
    }

    button:hover {
      background-color: #0056b3;
    }

    /* Styling for data values */
    .value {
      color: #4CAF50; /* Green for voltage/force values */
    }

    .warning {
      color: #f44336; /* Red for warning values */
    }

    /* Responsive design for mobile */
    @media (max-width: 600px) {
      h1 {
        font-size: 24px;
      }
      .container {
        padding: 15px;
      }
      button {
        width: 100%;
      }
    }

  </style>
</head>
<body>
  <div class="container">
    <h1>Force Sensor Data</h1>
    
    <div id="forceDisplay">
      <p>Analog Reading: <span id="fsrReading">--</span></p>
      <p>Voltage: <span id="voltage" class="value">--</span> V</p>
      <p>Force: <span id="force" class="value">--</span> N</p>
    </div>

    <button onclick="goToHome()">Home</button>
  </div>

  <script>
    var socket;

    function initWebSocket() {
      socket = new WebSocket('ws://' + window.location.hostname + ':81/');
      socket.onmessage = function(event) {
        var data = event.data.split(",");
        document.getElementById('fsrReading').innerText = data[0];
        document.getElementById('voltage').innerText = data[1];
        document.getElementById('force').innerText = data[2];

        // Apply different styles based on the value (e.g., if force is too high, show warning)
        var force = parseFloat(data[2]);
        if (force > 10) {
          document.getElementById('force').classList.add('warning');
        } else {
          document.getElementById('force').classList.remove('warning');
        }
      };

      socket.onerror = function(event) {
        console.error("WebSocket error:", event);
      };

      refreshForceData();
    }

    function refreshForceData() {
      if (socket.readyState === WebSocket.OPEN) {
        socket.send('refreshForce');
      } else {
        console.error("WebSocket connection is not open.");
      }
    }

    function goToHome() {
      window.location.href = "/"; // Change this to the actual home page URL if necessary
    }

    initWebSocket();
    setInterval(refreshForceData, 2000);  // Update every 2 seconds
  </script>
</body>
</html>
)rawliteral";
