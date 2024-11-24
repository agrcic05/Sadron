const char servo_control_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Servo Motor Control</title>
  <style>
    :root {
      --bg-color: linear-gradient(135deg, #C8102E, #54585A);
      --container-bg: #fff;
      --text-color: #333;
      --secondary-text-color: #666;
      --button-bg-color: #000;
      --button-hover-bg-color: #444;
      --button-text-color: #fff;
      --button-border-radius: 5px;
      --button-padding: 12px 20px;
      --button-font-size: 16px;
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

    /* Curved Slider */
    .arc-slider {
      position: relative;
      width: 200px;
      height: 100px;
      margin: 20px auto;
    }

    .arc-slider .arc {
      position: absolute;
      width: 100%;
      height: 100%;
      background: transparent;
      border-top: 6px solid #888;
      border-radius: 100px 100px 0 0;
    }

    .arc-slider .thumb {
      position: absolute;
      width: 20px;
      height: 20px;
      background-color: #000;
      border-radius: 50%;
      cursor: pointer;
      transform: translate(-50%, -50%); /* Perfect centering */
    }

    /* Back Button */
    #backButton {
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

    #backButton:hover {
      background-color: var(--button-hover-bg-color);
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>Servo Motor Control</h1>
    <p>Move the slider to set the servo position (-90° to +90°).</p>

    <!-- Servo Curved Slider -->
    <div class="arc-slider">
      <div class="arc"></div>
      <div class="thumb" id="sliderThumb" style="left: 100px; top: 6px;"></div>
    </div>
    <p>Servo Position: <span id="servoValue">0</span>°</p>

    <!-- Back Button -->
    <button id="backButton" onclick="goHome()">Home</button>
  </div>

  <script>
    const thumb = document.getElementById("sliderThumb");
    const servoValue = document.getElementById("servoValue");
    const ws = new WebSocket('ws://' + window.location.hostname + ':81/');
    let lastSentPosition = 90;

    const radius = 100; // Radius of the arc in pixels
    const arcOffset = 6; // Thickness of the arc border for alignment

    // Update thumb position and send value
function updateThumb(angle) {
  const radian = angle * (Math.PI / 180); // Convert angle to radians
  const x = radius - radius * Math.sin(radian); // Invert X-axis calculation
  const y = radius - radius * Math.cos(radian) + arcOffset; // Y position adjusted by arc offset
  thumb.style.left = `${x}px`;
  thumb.style.top = `${y}px`;
  servoValue.textContent = angle;
}

    // Handle dragging
    thumb.addEventListener("mousedown", () => {
      document.addEventListener("mousemove", moveThumb);
      document.addEventListener("mouseup", stopThumb);
    });

function moveThumb(event) {
  const rect = thumb.parentNode.getBoundingClientRect();
  const x = event.clientX - rect.left - radius;
  const y = radius - (event.clientY - rect.top);
  let angle = Math.round((Math.atan2(x, y) * 180) / Math.PI);
  if (angle < -90) angle = -90; // Limit west
  if (angle > 90) angle = 90;  // Limit east
  angle = -angle; // Invert the angle to correct direction
  updateThumb(angle);
  sendServoPosition(angle + 90); // Adjust to match servo range (0-180)
}

    function stopThumb() {
      document.removeEventListener("mousemove", moveThumb);
      document.removeEventListener("mouseup", stopThumb);
    }

    function sendServoPosition(position) {
      if (position !== lastSentPosition && ws.readyState === WebSocket.OPEN) {
        ws.send("servoPosition:" + position);
        lastSentPosition = position;
        console.log("Sent servo position: " + position);
      }
    }

    function goHome() {
      window.location.href = "/";
    }

    updateThumb(0); // Initialize thumb position at the top
  </script>
</body>
</html>
)rawliteral";
