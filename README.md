# Terrarium controller firmware

This project is a firmware for the Terrarium Controller board and some extension boards.
The main goal of this project is to provide reptiles with the closest environmental conditions to what they will have in the wild. We can control temperature, humidity, rain/fog, and light levels to achieve this.

## Features

### Climate

- Temperature control with PD and PID (experimental) options
    - Static temperature maintenance
    - Ramping up and down temperatures for 2 given points in time. (e.g. from 7.00 to 10.00, from 23C to 30C will re-calculate wanted temperature every 15s)
- Illumination control with day/night schedule
    - On/Off control
    - Dimming for AC (non-fluorescent lamps) with the dimmer extension module
- Humidity monitoring

### System

- WiFi for connectivity
    - Could work as a client
    - Could work as an Access point
- Status LED for observing if everything is fine
- Buzzer for audible notifications
- HTTP server with web interface and API for integrations
- OTA (Over the Air) updates of the system
- On-board time with NTP and RTC

### Supporter sensors

- DHT22 (temperature and humidity)
- BME280 (temperature and humidity)
- DS18B20 (temperature only)
- SHT31 (temperature and humidity)

## Installation

TODO

- Upload via OTA update
- Upload via USB (check for web-based uploads)
- Compile and upload via PlatformIO (link to section)

## Usage

TODO

- Onboarding process
- Explain features and how to use them

## Roadmap

If you are curious about the current status of the project and future improvements, check out our [Trello board](https://trello.com/b/peFyU6Do/terrarium-firmware)

## Contributing

(todo: add screenshots where needed)

In order to start developing this project you would need to

1. Clone this project
2. Install [VSCode](https://code.visualstudio.com/)
3. Install [Platform.IO](https://platformio.org/install/ide?install=vscode)
4. Open this project with VSCode
5. Open the PlatformIO plugin and find the component you want to build
6. Run build and then Upload and monitor.

## License

## Contact

[Our Discord #firmware](https://discord.gg/AaHFa7VVq8)

Grigorii Merkushev - [@brushknight](https://instagram.com/brushknight) - [brushknight@gmail.com](mailto:brushknight@gmail.com)

<p align="right">(<a href="#readme-top">back to top</a>)</p>
