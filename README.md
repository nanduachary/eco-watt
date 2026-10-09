AI-Powered Smart Electricity Management System for Campuses
An ESP32-based prototype that helps reduce unnecessary electricity use in campus rooms by combining occupancy sensing, ambient-light detection, temperature monitoring, relay-based appliance control, and electrical measurements.
> **Prototype status:** This repository documents a hardware prototype. Sensor calibration, occupancy behavior, electrical measurements, and relay operation must be validated on the actual hardware before any real-world deployment.
Features
Occupancy detection: PIR motion sensor and ultrasonic distance sensor.
Automatic lighting control: LDR digital output determines whether the three light relays are enabled while the room is considered occupied.
Temperature-based fan control: DHT11 temperature reading switches the fan relay at a configurable threshold.
Grace period: Loads are switched off after a configurable period without a detected occupancy event.
Energy telemetry: Reads analog inputs for voltage and current estimates and prints calculated power to Serial Monitor.
Relay status reporting: Reports each relay state along with sensor readings.
ESP32-based control: Decisions are made locally in the Arduino loop.
Hardware in this prototype
Component	Pin / connection in firmware	Purpose
PIR sensor output	GPIO 13	Motion detection
LDR module digital output	GPIO 14	Bright/dark signal
DHT11 data	GPIO 4	Temperature
Ultrasonic TRIG	GPIO 5	Distance measurement
Ultrasonic ECHO	GPIO 18	Distance measurement
Voltage-sensor output	GPIO 32 (ADC)	Voltage estimate
Current-sensor output	GPIO 35 (ADC)	Current estimate
Relay IN1	GPIO 25	Light 1
Relay IN2	GPIO 26	Light 2
Relay IN3	GPIO 27	Light 3
Relay IN4	GPIO 33	Fan
Pin assignments above preserve the supplied sketch. Verify every wire against the physical ESP32 board and your module labels before powering the system.
Repository layout
```text
smart-campus-electricity-management/
├── arduino/
│   └── smart_campus.ino
├── hardware/
│   └── README.md
├── docs/
│   └── README.md
├── .gitignore
└── README.md
```
Software requirements
Arduino IDE
ESP32 board support package for the specific ESP32 board
`DHT sensor library` (Adafruit)
`Adafruit Unified Sensor` (if required by the installed DHT library version)
`NewPing` library
Install libraries through Arduino IDE → Library Manager. Select the correct ESP32 board and port before uploading.
Upload and test
Open `arduino/smart_campus.ino` in Arduino IDE.
Confirm the board selection and serial port.
Install the libraries listed above.
Check the wiring and voltage levels, especially the ultrasonic ECHO and analog sensor inputs.
Upload the sketch.
Open Serial Monitor at 115200 baud.
Test each sensor separately, then test occupancy timing and relay outputs with low-voltage test loads first.
Control behavior
A HIGH PIR signal or a valid ultrasonic distance below 100 cm refreshes the occupancy timer.
The room is considered occupied for the configured grace period after the most recent event.
While occupied, the three light relays turn on when the LDR digital output is HIGH; otherwise they turn off.
While occupied, the fan relay turns on when a valid DHT11 reading is at or above the temperature threshold.
When the room is not considered occupied, all four relays are commanded off.
Important logic detail
The LDR rule depends on the module's digital-output polarity. The supplied sketch treats `HIGH` as the condition to turn lights on, while its Serial output labels `LOW` as “Bright”. Confirm the module behavior in your room and adjust the condition or wording so they agree.
Calibration and limitations
The voltage and current formulas in the firmware are assumptions, not universal sensor calibrations. They must be checked against the exact sensor modules, ESP32 ADC behavior, and a trusted meter.
The current formula assumes a midpoint bias of 1.65 V and sensitivity of 0.100 V/A. Those values may not match your hardware.
`voltage × current` is only a basic apparent/estimated power calculation for AC loads unless voltage, current, phase, and power factor are measured appropriately.
The current sensor type is not identified by the sketch. Confirm that its output is safe for the ESP32 ADC and that the sensor circuit is designed for the specific AC/DC measurement method.
PIR detects motion, not presence. A person sitting still may not continually trigger it; the ultrasonic sensor may not reliably detect every occupant or room layout.
The grace period is set to 15 seconds for testing. Increase it after testing; a longer delay such as several minutes is generally more appropriate for a room.
The `NewPing` library's compatibility with your exact ESP32 core version should be verified by compiling for your selected board.
Electrical safety
Do not connect exposed mains wiring on a breadboard or leave it accessible. Mains-powered lights and fans must be switched only with correctly rated hardware, appropriate fusing/protection, an insulated enclosure, strain relief, and suitable separation between mains and low-voltage circuits. Have mains wiring checked by a qualified person. Begin demonstrations with safe low-voltage loads.
The HC-SR04 ECHO output is commonly 5 V. ESP32 GPIOs are not 5 V tolerant; use an appropriate divider or level shifter if your specific sensor outputs 5 V. Ensure analog inputs never exceed the ESP32's allowed range.
Troubleshooting
No temperature value: Check DHT11 wiring, power, data pin, and library installation.
Distance always zero: Check TRIG/ECHO wiring, target range, sensor orientation, and ESP32 compatibility with NewPing.
Lights operate in reverse: Check whether the LDR module's digital output is HIGH in darkness or brightness.
Relay operates in reverse: This code assumes active-LOW relay inputs (LOW = ON, HIGH = OFF). Confirm the relay board's actual behavior.
Voltage/current readings look wrong: Calibrate the analog circuits; do not treat the default formulas as accurate measurements.
Future improvements
Add a manual override and a configurable grace period.
Use calibrated voltage/current measurement appropriate to the actual sensors.
Publish telemetry to MQTT and build a Node-RED dashboard.
Log readings and calculate energy savings from measured data.
Add fault handling and robust occupancy logic.
License
No license has been selected yet. Add a license file if you want to specify how others may use, modify, and redistribute this project.
