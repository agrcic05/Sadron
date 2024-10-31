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
const int ledPin = 18;
const int tempPin = 19;
const int fsrAnalogPin = 35;  // FSR connected to analog pin 4
const int motorPin = 32;     // Pin for DC motor control
const int servoPin = 26;     // Pin for Servo motor control
const int sdaPin = 21;       // SDA pin for I2C LCD
const int sclPin = 22;       // SCL pin for I2C LCD

int fsrReading;
float voltage;
float force;

Servo myServo;  // Create a Servo object (ESP32Servo)
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Initialize LCD (16x2 display)

// WiFi name and password
const char* ssid = "OSU_STEM";
const char* password = "123456789";

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
#include "lcd_display.h"    // LCD display HTML page

// Function to connect to WiFi and print the process to serial monitor
void connectToWiFi() {
  Serial.print("Connecting to WiFi");
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

  } else if (message == "toggleLED") {
    toggleLED();
    bool ledStatus = digitalRead(ledPin);
    webSocket.broadcastTXT(ledStatus == HIGH ? "ON" : "OFF");

  } else if (message == "refreshTemp") {
    float temperature = readTemperature();
    webSocket.broadcastTXT(String(temperature).c_str());

  } else if (message == "toggleMotor") {
    toggleMotor();  // Toggle the DC motor state

  } else if (message.startsWith("servoPosition:")) {
    int position = message.substring(14).toInt();  // Get the servo position
    if (position >= 0 && position <= 180) {        // Ensure the position is within range
      myServo.write(position);  // Move servo to the given position
      Serial.print("Moving servo to: ");
      Serial.println(position);
    } else {
      Serial.println("Invalid servo position received!");
    }

  } else if (message.startsWith("lcd:")) {
    String lcdText = message.substring(4);  // Extract text after "lcd:"
    updateLCD(lcdText);  // Update the LCD with the provided text
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

  // Initialize LED pin
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);  // Start with LED off

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
  server.on("/lcd_display", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", lcd_display_html);  // Serve new LCD control page
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
void toggleLED() {
  int ledState = digitalRead(ledPin);
  digitalWrite(ledPin, ledState == LOW ? HIGH : LOW);  // Toggle LED state
}

// Function to toggle the DC motor
void toggleMotor() {
  int motorState = digitalRead(motorPin);
  digitalWrite(motorPin, motorState == LOW ? HIGH : LOW);  // Toggle motor state
  webSocket.broadcastTXT(motorState == LOW ? "ON" : "OFF");
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