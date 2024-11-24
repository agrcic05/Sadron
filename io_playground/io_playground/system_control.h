const char system_control_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>System Control</title>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; }
        button { margin: 10px; padding: 10px; font-size: 18px; }
        .status { margin-top: 20px; }
    </style>
    <script>
    let socket = new WebSocket("ws://" + window.location.hostname + ":81/");
    let debounceTimeout;

    socket.onopen = function() {
        console.log("WebSocket connection established.");
        setInterval(refreshStatus, 2000); // Automatically refresh system status every 2 seconds
    };

    socket.onmessage = function(event) {
        let data = JSON.parse(event.data);
        document.getElementById("ledStatus").innerText = data.ledStatus;
        document.getElementById("motorStatus").innerText = data.motorStatus ? "ON" : "OFF";
        document.getElementById("servoStatus").innerText = data.servoRunning ? "Spinning" : "Stopped";
    };

    function toggleSystem() {
        if (debounceTimeout) return; // If a debounce timer is active, ignore the click

        socket.send("toggleSystem"); // Send toggle command to the server
        debounceTimeout = setTimeout(() => debounceTimeout = null, 500); // Shorter debounce (500ms)
    }

    function refreshStatus() {
        socket.send("getSystemStatus");
    }
</script>
</head>
<body>
    <h1>System Control</h1>
    <button onclick="toggleSystem()">Toggle Systems On/Off</button>
    <div class="status">
        <p>LEDs: <span id="ledStatus">Unknown</span></p>
        <p>DC Motor: <span id="motorStatus">Unknown</span></p>
        <p>Servo Motor: <span id="servoStatus">Unknown</span></p>
    </div>
</body>
</html>
)rawliteral";
