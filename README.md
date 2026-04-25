# Smart Health Monitoring System using IoT

The Smart Health Monitoring System is an IoT-based project that monitors key health parameters in real time. It measures **heart rate**, **SpO₂**, and **body temperature** using an **Arduino Nano** with a **MAX30100 pulse oximeter sensor** and an **LM35 temperature sensor**. The readings are sent to a **NodeMCU (ESP8266)**, which creates a wireless access point and hosts a simple dashboard for live monitoring.  

## Features
- Real-time monitoring of heart rate, SpO₂, and body temperature
- Arduino Nano-based sensor data acquisition
- NodeMCU-hosted wireless dashboard
- Serial communication between Nano and NodeMCU
- LCD display for on-device output
- Beat detection indicator using an onboard LED

## Hardware Used
- Arduino Nano
- NodeMCU (ESP8266)
- MAX30100 Pulse Oximeter Sensor
- LM35 Temperature Sensor
- 16x2 LCD Display
- Connecting wires
- Power supply

## Software Used
- Arduino IDE
- C++ (Arduino sketches)
- Embedded HTML for the dashboard

## Working Principle
1. The **Arduino Nano** reads data from the MAX30100 sensor and the LM35 sensor.
2. Heart rate, SpO₂, and temperature values are displayed on the LCD.
3. The Nano sends the readings to the **NodeMCU** through serial communication.
4. The NodeMCU processes the incoming data and serves it on a local web dashboard.
5. Users can connect to the NodeMCU Wi-Fi network and view the live health values in a browser.

## Project Structure
- `nano_sensor_module.ino` — reads sensor data from the Nano side
- `nodemcu_health_monitor.ino` — creates the Wi-Fi dashboard on ESP8266
- `images/` — contains the system diagram
- `README.md` — project documentation

## How It Works
- The Nano collects sensor readings every few seconds.
- The pulse oximeter library calculates heart rate and SpO₂.
- The LM35 sensor provides body temperature in °C.
- The NodeMCU receives the values and exposes them on a simple web page.

## Future Improvements
- Add cloud storage for health records
- Send SMS/email alerts for abnormal readings
- Add graph-based visualization
- Integrate mobile app support
- Improve sensor calibration and accuracy

## Conclusion
This project demonstrates how IoT can be used for affordable and real-time health monitoring. It combines sensing, serial communication, and web-based visualization into a compact embedded system.

![System Diagram](https://github.com/suyashagnihotri/Smart-health-monitoring-System-using-IOT/blob/main/diagram%20(13).png)


