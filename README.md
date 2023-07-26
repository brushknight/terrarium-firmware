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

- [DHT22](https://www.dfrobot.com/product-1102.html) (temperature and humidity)
- [BME280](https://www.amazon.es/AZDelivery-GY-BME280-Sensor-Parent/dp/B082KL18WW) (temperature and humidity)
- [DS18B20](https://www.dfrobot.com/product-689.html) (temperature only)
- [SHT31](https://www.dfrobot.com/product-2160.html) (temperature and humidity)

# Installation

TODO

- Upload via OTA update
- Upload via USB (check for web-based uploads)
- Compile and upload via PlatformIO (link to section)

# Usage

## Initial setup

1. When you start controller for the first time, it will boot in standalone mode. Indication of this mode is purple light near the usb-c
2. When in standalone mode, you need to connecto to newly created WiFi network names "Terratium Controller 00000000" where numbers is the mac-address of your controller. Passphrase is "Chameleon"
3. Once connected, navigate to http://192.168.4.1 and you will be welcomed with [setup screen](./docs/initial_setup.png) where you can specify (Wifi network to connect, time-zone, NTP - Network time sync, ID of the controller so you can identify it later and animal name for the same reason). Onse saved controller will stop internal WiFi point and will connect to the given WiFi.
4. You will need to navigate to your router and find the controller IP. Recommended to pin it, so it will be always the same.

## Climate setup

1. Navigate to http://IP_OF_YOU_CONTROLLER/api/climate to obtain current climate settings JSON file
2. This JSON consists of multiple parts: temperature control zones, dimmer control zones (in current implementation of hardware there is no dimmer, just on/off switch), color light zones (Light output) for LEDs, fan zones
3. Copy JSON into editor, for example https://jsonformatter.org/
4. Once JSON is updated, navigate to http://IP_OF_YOU_CONTROLLER/climate and submit the form with the JSON
5. Controller will immediately apply the new config, but will take a few menites to save it to the internal storage.

### Temperature zone
```
{"slug":"hot_side","enabled":true,"sensor_ids":[{"port":0,"type":1820},{"port":1,"type":1820},{"port":-1,"type":-1}],"events":[{"set":true,"since":"10:00","until":"18:00","duration_sec":-1,"temperature":35,"mode":0,"transform":{"from":0,"to":0}},{"set":true,"since":"18:00","until":"20:00","duration_sec":-1,"temperature":-1,"mode":0,"transform":{"from":35,"to":22}},{"set":true,"since":"20:00","until":"07:00","duration_sec":-1,"temperature":22,"mode":0,"transform":{"from":0,"to":0}},{"set":true,"since":"07:00","until":"10:00","duration_sec":-1,"temperature":-1,"mode":0,"transform":{"from":22,"to":35}},{"set":false,"since":"00:00","until":"00:00","duration_sec":-1,"temperature":-1,"mode":-1515870811,"transform":{"from":0,"to":0}}],"heater_port":0}
```
- `slug`: just a name for this zone, used in the report page
- `enabled`: if this zone is enabled or not
- `sensor_ids`: array of sensors that will be used in aggregation mode, can be 1,2 or 3 sensors per zone
- `events`: schedule of desired temperatures
- `heater_port`: load output id to be used in this zone

sensors_id: `{"port":0,"type":1820}`
- `port`: physical sensors output 
- `type`: type of the sensors, can be 22 (DHT22), 31 (SHT31), 280 (BME280), 1820 (DS18B20)

events: `{"set":true,"since":"10:00","until":"18:00","duration_sec":-1,"temperature":35,"mode":0,"transform":{"from":0,"to":0}}`
- `set`: is this event enabled
- `since`: HH:MM from what time this event is activated
- `until`: HH:MM until what time this event is activated
- `duration_sec`: not in use
- `temperature`: desired temperature
- `mode`: not in use (will be possible to change between PD and PID in the future)
- `transform`: make a temperature ramp-up, rump-down linearly during the duration of the event

### Dimmer zones
```
{"slug":"UVB","enabled":true,"events":[{"set":true,"type":"simple","since":"08:00","until":"17:00","duration_sec":-1,"brightness":100,"color":{"r":0,"g":0,"b":0,"k":0},"transform":{"from":0,"to":0}},{"set":true,"type":"simple","since":"17:00","until":"08:00","duration_sec":-1,"brightness":0,"color":{"r":0,"g":0,"b":0,"k":0},"transform":{"from":0,"to":0}},{"set":false,"type":"simple","since":"00:00","until":"00:00","duration_sec":-1,"brightness":0,"color":{"r":0,"g":0,"b":0,"k":0},"transform":{"from":0,"to":0}},{"set":false,"type":"simple","since":"00:00","until":"00:00","duration_sec":-1,"brightness":0,"color":{"r":0,"g":0,"b":0,"k":0},"transform":{"from":0,"to":0}},{"set":false,"type":"simple","since":"00:00","until":"00:00","duration_sec":-1,"brightness":0,"color":{"r":0,"g":0,"b":0,"k":0},"transform":{"from":0,"to":0}}],"dimmer_port":2}
```
- `slug`: just a name for this zone, used in the report page
- `enabled`: if this zone is enabled or not
- `events`: schedule of desired temperatures
- `dimmer_port`: load output id to be used in this zone

events: `{"set":true,"type":"simple","since":"08:00","until":"17:00","duration_sec":-1,"brightness":100,"color":{"r":0,"g":0,"b":0,"k":0},"transform":{"from":0,"to":0}}`
- `set`: is this event enabled
- `type`: not in use
- `since`: HH:MM from what time this event is activated
- `until`: HH:MM until what time this event is activated
- `duration_sec`: not in use
- `brightness`: desired brightness (0 - off, 1-100 - on due to lack of dimmer hardware)
- `color`: not in use
- `transform`: not in use due to lack of dimmer hardware

### Color light zone
```
{"slug":"","enabled":false,"events":[{"set":false,"type":"simple","since":"00:00","until":"00:00","duration_sec":-1,"brightness":0,"color":{"r":0,"g":0,"b":0,"k":0},"transform":{"from":0,"to":0}},{"set":false,"type":"simple","since":"00:00","until":"00:00","duration_sec":-1,"brightness":0,"color":{"r":0,"g":0,"b":0,"k":0},"transform":{"from":0,"to":0}},{"set":false,"type":"simple","since":"00:00","until":"00:00","duration_sec":-1,"brightness":0,"color":{"r":0,"g":0,"b":0,"k":0},"transform":{"from":0,"to":0}},{"set":false,"type":"simple","since":"00:00","until":"00:00","duration_sec":-1,"brightness":0,"color":{"r":0,"g":0,"b":0,"k":0},"transform":{"from":0,"to":0}},{"set":false,"type":"simple","since":"00:00","until":"00:00","duration_sec":-1,"brightness":0,"color":{"r":0,"g":0,"b":0,"k":0},"transform":{"from":0,"to":0}}],"actuator":{"type":-1,"i2c_id":0}}
```
- `slug`: just a name for this zone, used in the report page
- `enabled`: if this zone is enabled or not
- `events`: schedule of desired temperatures
- `actuator`: load identification (can be onboard LED or extension module)

events: `{"set":false,"type":"simple","since":"00:00","until":"00:00","duration_sec":-1,"brightness":0,"color":{"r":0,"g":0,"b":0,"k":0},"transform":{"from":0,"to":0}}`
- `set`: is this event enabled
- `type`: `simple` or `circadian` (will change color temperature during the cycle)
- `since`: HH:MM from what time this event is activated
- `until`: HH:MM until what time this event is activated
- `duration_sec`: not in use
- `brightness`: desired brightness
- `color`: wanted color (for simple mode)
- `transform`: rump-up rump-down for brightness linearly during the duration of the event

actuator: `{"type":-1,"i2c_id":0}`
- `type`: 0 - on board, 1 - light dome, 2 - dimmer module
- `i2c_id`: ID of the external module 


## Sensors endpoint

http://IP_OF_YOU_CONTROLLER/api/sensors

```
{"sensors":[{"port":1,"type":"BME280","humidity":38.70117188,"temperature":26.84000015},{"port":2,"type":"BME280","humidity":0,"temperature":0},{"port":2,"type":"DS18B20","humidity":0,"temperature":25.75},{"port":3,"type":"DS18B20","humidity":0,"temperature":24.6875},{"port":4,"type":"DS18B20","humidity":0,"temperature":25.75},{"port":5,"type":"DS18B20","humidity":0,"temperature":25.4375}]}
```
- `port`: physical sensors port
- `type`: type of the sensors
- `humidity`: Humidity value or 0 is not supported by the given sensor
- `temperature`: Temperature 

## Metrics endpoint

http://IP_OF_YOU_CONTROLLER/api/metrics

```
{"metadata":{"wifi":"EDITED","mac":"3485187AE338","time":{"hour":11,"minute":26,"uptime":204},"build_time":1689539428},"system":{"rtc":{"percent":129,"mV":3131}},"climate":{"temperature_zones":{"hot_side":{"heater_status":false,"temperature_error":0,"error":"ERROR: no temperature sensros found","timestamp":0},"chill_side":{"average_temperature":24.875,"target_temperature":26,"heater_status":true,"temperature_error":0.375,"timestamp":1690363562}},"dimmer_zones":{"UVB":{"brightness":100},"HEAT_bulb":{"brightness":100}},"fan_zones":{"air_circulation":{"power":100},"humidity":{"power":0}}}}
```

- `metadata`: Some generic info about the controller: WiFi, time, firmware version
- `system`: information about the system: RTC battery status (malfunctioning on the v0.12-rc2 board version)
- `climate`: everything currently active about the climate

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
