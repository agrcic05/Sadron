#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>

// Define GPIO pins
const int ledPin = 18;
const int tempPin = 19;

// WiFi name and password
const char* ssid = "Ohio Stater_Resident_WiFi"; 
const char* password = "lemon377common";

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

// Arduino required setup function
void setup() {
  // Connects to serial monitor
  Serial.begin(115200);

  // Start WiFi setup
  connectToWiFi();

  // Initialize LED pin
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // Starts off

  // Start DallasTemperature sensor
  sensors.begin();

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

// Function to connect to WiFi and print the process to serial monitor
void connectToWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password); // Connect to the WiFi network

  while (WiFi.status() != WL_CONNECTED) { // Loop until connected
    Serial.print(".");
    delay(1000);
  }

  Serial.println("");
  Serial.println("Connection successful!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// Function to forward WebSocket actions and request to correct 
void handleWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  switch(type) {
    case WStype_TEXT:
      // Handle text message from WebSocket
      handleWebSocketText(payload, length);
      break;
    case WStype_BIN:
      // Handle binary message from WebSocket
      handleWebSocketMessage(num, payload, length);
      break;
    default:
      break;
  }
}

// Function to parse WebSocket messages and respond accordingly
void handleWebSocketText(uint8_t *payload, size_t length) {
  String message = "";
  for (size_t i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  // Prints WebSocket messages to serial monitor for debugging
  Serial.print("Received WebSocket message: ");
  Serial.println(message);
  
  // Handles toggle LED button requests
  if (message == "toggleLED") {
    // Toggles LED
    toggleLED();
    // More debugging messages
    bool ledStatus = digitalRead(ledPin);
    Serial.print("LED set to: ");
    Serial.println(ledStatus);
    // Send the current LED state back to the client
    webSocket.broadcastTXT(digitalRead(ledPin) == HIGH ? "ON" : "OFF");
  // Handles refresh temperature requests
  } else if (message == "refreshTemp") {
    // More debigging messages
    float temperature = readTemperature();
    Serial.print("Temperature reading: ");
    Serial.println(temperature);
    // Send the current temperature back to the client
    webSocket.broadcastTXT(String(temperature).c_str());
  // Handles requests for the current state of the LED
  } else if (message == "getLEDState") {
    bool ledStatus = digitalRead(ledPin);
    // More debugging messages
    Serial.print("LED: ");
    Serial.println(ledStatus);
    // Send the current LED state back to the client
    webSocket.broadcastTXT(digitalRead(ledPin) == HIGH ? "ON" : "OFF");
  }
}

// Function that reports back to webpage that the message was recieved
void handleWebSocketMessage(uint8_t num, uint8_t *payload, size_t length) {
  // Handle WebSocket message here
  webSocket.sendTXT(num, "Message received!");
}

// Toggles LED On/Off
void toggleLED() {
  digitalWrite(ledPin, !digitalRead(ledPin));
}

// Reads and returns current temperature
float readTemperature() {
  sensors.requestTemperatures();
  // Can change temperature unit to C/F here but the range of the Thermometer on the webpage needs reconfigured
  return sensors.getTempCByIndex(0);
}
