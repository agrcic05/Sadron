# Sadron IO Playground

The IO Playground is a tool for students to learn about microcontrollers and how they work. It provides a platform for monitoring temperature readings from a DS18B20 temperature sensor. You can also toggle an LED connected to the ESP32 and view its current state. All of this is done through a web interface hosted on the ESP32.

## Features

- Real-time temperature monitoring using a DS18B20 temperature sensor
- Visual representation of temperature on a thermometer
- LED control: toggle an LED connected to the ESP32
- Responsive web interface accessible from any device with a web browser
- WebSocket communication for real-time data updates

## Hardware Requirements

- ESP32 microcontroller board
- DS18B20 temperature sensor
- LED

## Software Requirements

- Arduino IDE
- Libraries: ESPAsyncWebServer, WebSockets, OneWire, DallasTemperature

## Installation

1. Clone or download this repository to your local machine
2. Open the project in Arduino IDE
3. Install the required libraries if not already installed
4. Connect your ESP32 board to your computer via USB
5. Upload the sketch to your ESP32 board
6. Once uploaded, open the Serial Monitor to view the IP address of your ESP32
7. Open a web browser and enter the IP address to access the web interface

## Usage

1. **Temperature Monitoring**: Visit the `/thermometer` route to view the thermometer visualization and real-time temperature reading
2. **LED Control**: Visit the `/led_control` route to toggle the LED connected to the ESP32
3. **WebSocket Communication**: The web interface uses WebSocket communication for real-time updates. No manual refreshing is required

## Meet the Team

Team Sadron is made up of 5 undergraduate sudents from The Ohio State University. We are working on this project in colaberation with the OSU STEM Outreach Program and Capstone instructor Alan Gilbert.

Members:
- Angelo Dallas
	- Electrical Engineering Major
- Matthew Disanto
	- Computer Engineering Major
- Andrew Grcic
	- Computer Engineering Major
- Ellie Rannebarger
	- Electrical Engineering Major
- Kyle Saul
	- Computer Engineering Major

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE.txt) file for details.
