#include <LiquidCrystal_I2C.h>

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <ESP32Servo.h>  // Include ESP32Servo library
#include <Wire.h>
#include <LiquidCrystal_I2C.h>  // Include LiquidCrystal_I2C for LCD

// Define GPIO pins
const int ledPins[] = {2, 4, 15, 18, 12, 23};  // Update these pin numbers according to your setup
const int tempPin = 19;
const int fsrAnalogPin = 35;  // FSR connected to analog pin 4
const int motorPin = 32;     // Pin for DC motor control
const int servoPin = 26;     // Pin for Servo motor control
const int sdaPin = 21;       // SDA pin for I2C LCD
const int sclPin = 22;       // SCL pin for I2C LCD

#define DEBOUNCE_DELAY 12500  // Debounce delay in milliseconds
int fsrReading;
float voltage;
float force;
float temperatureThreshold = 30.0;  // Default threshold is 30°C
unsigned long lastLEDToggleTime = 0;
const unsigned long ledDebounceDelay = 200; // Debounce delay in milliseconds
// Global variables for system state


Servo myServo;  // Create a Servo object (ESP32Servo)
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Initialize LCD (16x2 display)

// WiFi name and password
const char* ssid = "OSU_STEM";
const char* password = "123456789";
// const char* ssid = "SpectrumSetup-7B";
// const char* password = "oceanwagon217";

// Initialize OneWire and DallasTemperature sensors
OneWire oneWire(tempPin);
DallasTemperature sensors(&oneWire);

// Initialize WebServer and WebSocket
AsyncWebServer server(80);
WebSocketsServer webSocket(81);

// Include required HTML pages
#include "index.h"
#include "led_control.h"
#include "thermometer.h"
#include "force_sensor.h"   // Force sensor HTML page
#include "dc_motor.h"       // DC motor HTML page
#include "servo_control.h"  // Servo motor control HTML page
//#include "lcd_display.h"    // LCD display HTML page
#include "dc_motor_temp.h"
#include "pressure_led_control.h"
#include "meet_the_team.h"

// Static IP, Gateway, and Subnet
IPAddress local_IP(192, 168, 0, 100);  // Your desired static IP address
IPAddress gateway(192, 168, 0, 1);    // Your network's gateway
IPAddress subnet(255, 255, 255, 0);   // Your network's subnet mask

// Function to connect to WiFi and print the process to serial monitor
void connectToWiFi() {
  Serial.print("Connecting to WiFi");

  // Configure static IP
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("Failed to configure Static IP");
  }

  WiFi.begin(ssid, password);  // Connect to the WiFi network

  while (WiFi.status() != WL_CONNECTED) {  // Loop until connected
    Serial.print(".");
    delay(1000);
  }

  Serial.println("");
  Serial.println("Connection successful!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// Function to forward WebSocket actions and request to correct handlers
void handleWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_TEXT:
      handleWebSocketText(payload, length);
      Serial.println();
      handleWebSocketText(payload, length);
      break;
    case WStype_BIN:
      handleWebSocketMessage(num, payload, length);
      break;
    default:
      break;
  }
}

// Handle WebSocket messages for force sensor, DC motor, servo control, and LCD
void handleWebSocketText(uint8_t *payload, size_t length) {
  String message = "";
  for (size_t i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Received WebSocket message: ");
  Serial.println(message);

  if (message == "refreshForce") {
    String forceData = readForceSensor();  // Read the sensor values
    webSocket.broadcastTXT(forceData);     // Send the voltage and force values to client

  } else if (message.startsWith("toggleLED")) {
    int ledNumber = message.substring(9).toInt();  // Extract LED number from message
    if (ledNumber >= 1 && ledNumber <= 6) {  // Ensure it's within range
      toggleLED(ledNumber);
      bool ledStatus = digitalRead(ledPins[ledNumber - 1]);
      webSocket.broadcastTXT(ledStatus == HIGH ? "ON" : "OFF");
    }
  }
    else if (message == "refreshTemp") {
    float temperature = readTemperature();
    webSocket.broadcastTXT(String(temperature).c_str());

  } else if (message == "toggleMotor") {
    toggleMotor();  // Toggle the DC motor state
  }  else if (message.startsWith("servoPosition:")) {
    int position = message.substring(14).toInt();  // Get the servo position
    if (position >= 0 && position <= 180) {        // Ensure the position is within range
      myServo.write(position);  // Move servo to the given position
      Serial.print("Moving servo to: ");
      Serial.println(position);
    } else {
      Serial.println("Invalid servo position received!");
    }

  } else if (message.startsWith("setThreshold:")) {
    temperatureThreshold = message.substring(13).toFloat();  // Extract threshold value
    Serial.print("New temperature threshold: ");
    Serial.println(temperatureThreshold);
  }
  else if (message.startsWith("lcd:")) {
    String lcdText = message.substring(4);  // Extract text after "lcd:"
    updateLCD(lcdText);  // Update the LCD with the provided text
  } else if (message.startsWith("pressureLED")) {
    updateLEDsBasedOnPressure();
  }
  else if (message == "refreshData") {
  float temperature = readTemperature();
  bool motorOn = false;

  // Check if the temperature exceeds the user-set threshold to turn on the motor
  if (temperature > temperatureThreshold) {
    digitalWrite(motorPin, HIGH);  // Turn the motor on
    motorOn = true;
  } else {
    digitalWrite(motorPin, LOW);   // Turn the motor off
    motorOn = false;
  }

  // Create JSON data string with temperature and motor state
  String jsonData = "{";
  jsonData += "\"temperature\":" + String(temperature) + ",";
  jsonData += "\"motorOn\":" + String(motorOn ? "true" : "false");
  jsonData += "}";

  // Send the JSON data to WebSocket
  webSocket.broadcastTXT(jsonData);
  }
 else if (message == "getLEDStates") {
    // Collect LED statuses into a string
    String ledStatuses = "{";
    for (int i = 0; i < 6; i++) {
        // Use numeric keys for compatibility with JavaScript
        ledStatuses += "\"" + String(i + 1) + "\":\"" + (digitalRead(ledPins[i]) == HIGH ? "ON" : "OFF") + "\"";
        if (i < 5) {
            ledStatuses += ","; // Add a comma between items
        }
    }
    ledStatuses += "}";

    // Send the LED statuses as a single string
    webSocket.broadcastTXT(ledStatuses);
}
else if (message == "checkMotorStatus") {
    int motorState = digitalRead(motorPin);  // Read the motor pin state
    if (motorState == HIGH) {
        webSocket.broadcastTXT("ON");  // Motor is on
    } else {
        webSocket.broadcastTXT("OFF"); // Motor is off
    }
}
}


// Function to update LCD display
void updateLCD(String text) {
  lcd.clear();
  lcd.setCursor(0, 0);  // Set cursor to first row
  lcd.print(text);      // Print the text on LCD
}

// Arduino required setup function
void setup() {
  // Connect to serial monitor
  Serial.begin(115200);

  // Start WiFi setup
  connectToWiFi();

  // Initialize LED pins
  for (int i = 0; i < 6; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);  // Start with all LEDs off
  }

  // Set motor pin as output
  pinMode(motorPin, OUTPUT);
  digitalWrite(motorPin, LOW);  // Start with motor off

  // Initialize the servo motor and set it to the middle position (90 degrees)
  ESP32PWM::allocateTimer(0);   // Allocate PWM timer 0 for servo
  ESP32PWM::allocateTimer(1);   // Allocate PWM timer 1 (reserve an extra timer)
  
  myServo.setPeriodHertz(50);   // Standard 50Hz frequency for servos
  myServo.attach(servoPin, 500, 2400);  // Attach servo to pin with custom pulse width range
  
  myServo.write(90);            // Move the servo to the default position (90 degrees)

  // Start DallasTemperature sensor
  sensors.begin();

  // Initialize LCD
  lcd.init();
  lcd.backlight();

    // Perform startup tasks (blink LEDs, turn on motor and servo)
  performStartupTasks();

  // Configure server routes for different pages
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });
  server.on("/led_control", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", led_control_html);
  });
  server.on("/thermometer", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", thermometer_html);
  });
  server.on("/force_sensor", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", force_sensor_html);
  });
  server.on("/dc_motor", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", dc_motor_html);
  });
  server.on("/servo_control", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", servo_control_html);
  });
  //server.on("/lcd_display", HTTP_GET, [](AsyncWebServerRequest *request) {
  //  request->send_P(200, "text/html", lcd_display_html);  // Serve new LCD control page
  //});
    server.on("/dc_motor_temp", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", dc_motor_temp_html); 
  });
  server.on("/pressure_led_control", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", pressure_led_control_html);
  });
    server.on("/meet_the_team", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", meet_the_team_html);
  });

  // Start web server
  server.begin();

  // Start WebSocket server
  webSocket.begin();
  webSocket.onEvent(handleWebSocketEvent);
}

// Arduino required loop function
void loop() {
  // Constantly running WebSocket
  webSocket.loop();
}

// Function to toggle the LED
int toggleLED(int ledNumber) {
  // Debounce check
  unsigned long currentTime = millis();
  if (currentTime - lastLEDToggleTime < ledDebounceDelay) {
    return 0; // Ignore if called within debounce delay
  }
  lastLEDToggleTime = currentTime; // Update the last toggle time

  // Toggle LED state
  int ledPin = ledPins[ledNumber - 1];  // Get the specific LED pin
  int ledState = digitalRead(ledPin);
  digitalWrite(ledPin, ledState == LOW ? HIGH : LOW);  // Toggle LED state

  // Send the current LED status to clients
  bool ledStatus = digitalRead(ledPin);
  webSocket.broadcastTXT(ledStatus == HIGH ? "ON" : "OFF");

  return 1;
}

// Global variable to track motor state
bool motorState = false; // False = OFF, True = ON
unsigned long lastToggleTime = 0;
const unsigned long debounceDelay = 200;  // Debounce delay in milliseconds

// Function to toggle the DC motor with debounce
void toggleMotor() {
  // Debounce check
  unsigned long currentTime = millis();
  if (currentTime - lastToggleTime < debounceDelay) {
    return;  // Ignore if called within debounce delay
  }
  lastToggleTime = currentTime;  // Update the last toggle time

  motorState = !motorState;  // Toggle motor state
  digitalWrite(motorPin, motorState ? HIGH : LOW);  // Apply new state to motor pin
  Serial.print("Toggling Motor. New state: ");
  Serial.println(motorState ? "ON" : "OFF");

  // Send current motor status to clients
  webSocket.broadcastTXT(motorState ? "ON" : "OFF");
}

void handleWebSocketMessage(uint8_t num, uint8_t *payload, size_t length) {
  webSocket.sendTXT(num, "Message received!");
}

float readTemperature() {
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0);
}

// Updated function to read force sensor value, convert to voltage and force
String readForceSensor() {
  // Read from the force sensor connected to analog pin 4
  fsrReading = analogRead(fsrAnalogPin);  

  // Convert the analog reading to voltage (using 3.3V reference for ESP32)
  float voltage = fsrReading * (3.3 / 4095.0);  // ESP32 uses 12-bit ADC (0-4095)

  // Calculate force based on the voltage (adjust the equation as per sensor datasheet)
  float force = calculateForceFromVoltage(voltage);

  // Print the values for debugging
  Serial.print("Analog reading = ");
  Serial.println(fsrReading);
  Serial.print("Voltage = ");
  Serial.println(voltage);
  Serial.print("Force = ");
  Serial.println(force);

  // Return force and voltage in a formatted string (for WebSocket update)
  return String(fsrReading) + "," + String(voltage) + "," + String(force);
}

// Function to handle pressure sensor LED updates
void updateLEDsBasedOnPressure() {
    // Read voltage from the pressure sensor
    fsrReading = analogRead(fsrAnalogPin);
    voltage = fsrReading * (3.3 / 4095.0);

    // Convert voltage to force
    force = calculateForceFromVoltage(voltage);

    // Set LED states based on voltage thresholds
    const float thresholds[] = {0.10, 0.15, 0.25, 0.35, 0.50, 0.70};
    String ledStatus = "[";  // JSON-like array for LED status
    for (int i = 0; i < 6; i++) {
        bool ledState = voltage >= thresholds[i];
        digitalWrite(ledPins[i], ledState ? HIGH : LOW);
        ledStatus += (ledState ? "\"ON\"" : "\"OFF\"");
        if (i < 5) ledStatus += ","; // Add comma between statuses
    }
    ledStatus += "]";

    // Create JSON data with force and LED status
    String jsonData = "{";
    jsonData += "\"force\":" + String(force) + ",";
    jsonData += "\"voltage\":" + String(voltage) + ",";
    jsonData += "\"ledStatus\":" + ledStatus;
    jsonData += "}";

    // Broadcast updated data via WebSocket
    webSocket.broadcastTXT(jsonData);

    // Print for debugging
    Serial.println(jsonData);
}
// void refreshData() {
//   float temperature = readTemperature();
//   bool motorOn = false;

//   // Check if the temperature exceeds the user-set threshold to turn on the motor
//   if (temperature > temperatureThreshold) {
//     digitalWrite(motorPin, HIGH);  // Turn the motor on
//     motorOn = true;
//   } else {
//     digitalWrite(motorPin, LOW);   // Turn the motor off
//     motorOn = false;
//   }

//   // Create JSON data string with temperature and motor state
//   String jsonData = "{";
//   jsonData += "\"temperature\":" + String(temperature) + ",";
//   jsonData += "\"motorOn\":" + String(motorOn ? "true" : "false");
//   jsonData += "}";

//   // Send the JSON data to WebSocket
//   webSocket.broadcastTXT(jsonData);
// }
// Function to calculate force based on voltage (you can adjust based on your sensor's curve)
float calculateForceFromVoltage(float voltage) {
  // Adjust this based on your sensor's datasheet or calibration curve
  if (voltage == 0) {
    return 0;  // No force applied
  }

  // Convert voltage to force (adjust this based on your sensor's specifications)
  float force = voltage * 55;  // Example multiplier, replace with actual sensor calibration

  return force;
}

// Function to perform initialization tasks at startup
void performStartupTasks() {
  // Blink all LEDs
  for (int i = 0; i < 6; i++) {
    digitalWrite(ledPins[i], HIGH);  // Turn LED on
    delay(200);                      // Wait for 200ms
    digitalWrite(ledPins[i], LOW);   // Turn LED off
    delay(200);                      // Wait for 200ms
  }

  // Turn on DC motor for 3 seconds
  digitalWrite(motorPin, HIGH);  // Turn motor on
  delay(3000);                   // Wait for 3 seconds
  digitalWrite(motorPin, LOW);   // Turn motor off

  // Turn on Servo motor for 3 seconds (move servo to 180 degrees)
  myServo.write(180);            // Move servo to 180 degrees (fully turned)
  delay(1000);                   // Wait for 3 seconds
  myServo.write(90);             // Move servo back to 90 degrees (default position)
  delay(1000);
  myServo.write(0);
  delay(1000);
  myServo.write(90);
}

