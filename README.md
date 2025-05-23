# Smart Monitor

**Smart Monitor** is an indigenous IoT product designed to monitor energy consumption along with additional features such as weather monitoring, date, and time display. This device leverages open-source libraries and custom firmware with Over-The-Air (OTA) updates to eliminate wired communication, enabling seamless firmware upgrades for continuous feature development and integration within a complete smart ecosystem.

---

## Features

- Real-time monitoring of energy consumption (voltage, current, power)
- Local temperature and humidity measurement
- Weather updates using OpenWeatherMap API
- Date and time synchronization via NTP server
- OTA firmware updates for hassle-free enhancements
- Web interface hosted on the device for power monitoring
- Non-blocking, efficient firmware using timers, counters, and interrupts

---

## Hardware Components

- **ESP8266EX** — 32-bit Tensilica® processor-based microcontroller  
- **LCD 20x4** — for local data display (I2C communication)  
- **DHT11** — temperature and humidity sensor  
- **PZEM-004T** — voltage and current sensor (UART communication)  

---

## Software and Libraries Used

- **Firmware:** Developed using advanced embedded concepts (non-blocking methods, timers, counters, interrupts)  
- **OTA Updates:** [Elegant OTA](https://docs.elegantota.pro/) — open-source library for wireless firmware updates  
- **Weather Data:** OpenWeatherMap API  
- **Time Sync:** NTP server  
- **Web Interface:** Power monitoring webpage adapted from [HowToElectronics](https://howtoelectronics.com)  

---

## Installation and Setup

1. Install dependencies and libraries (e.g., Elegant OTA, DHT11 library, etc.) through your preferred IDE (e.g., Arduino IDE).

2. Configure WiFi credentials and OpenWeatherMap API key in the firmware source code.

3. Flash the firmware to ESP8266EX using OTA or wired connection for first upload.

4. Access the web interface via the ESP8266 IP address to monitor data.

## Usage

- View energy consumption, temperature, humidity, weather data, date, and time on the LCD display.

- Access the hosted web page on ESP8266 for detailed power monitoring.

- OTA updates allow you to upload new firmware versions without physical connection.

## Project Status

This project is currently active and under continuous development for additional features and optimizations. This have both IoT and IoT firmware. 

## Contribution

Contributions are welcome! Feel free to open issues or submit pull requests with improvements.

##License

This project is licensed under the MIT License. See the LICENSE file for details.

## Author

Vignesh Sundar
LinkedIn: https://www.linkedin.com/in/vignesh-sundar-m/

## References

Elegant OTA Library

OpenWeatherMap API

HowToElectronics for Webpage


