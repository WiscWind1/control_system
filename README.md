# control_system

Arduino code for a wind turbine control system, managing turbine operation, power measurement, emergency braking, and blade pitch control.

## Features

- **Serial Communication:** Data exchange between two Arduinos and a PC.
- **Power Measurement:** Uses an INA260 sensor to monitor voltage, current, and power.
- **Emergency Braking:** Engages a relay-based brake during emergency stops.
- **Load Control:** Switches electrical load based on RPM and voltage.
- **Blade Pitch Control:** Uses PID control to adjust a servo for optimal RPM.

## Requirements

- Arduino Mega and Uno
- Adafruit INA260 sensor
- Relay module
- Servo motor
- Push button for emergency stop

## Instructions

1. Connect components as defined in the code.
2. Load the "Load Side" code on the Mega, and the "Turbine Side" code on the Uno.
3. Monitor via the serial output for system status.

## Next Steps

- Consider adding data logging or advanced control features.
- **Can we try with Python?** Exploring a Python-based implementation on a platform like Raspberry Pi could offer more flexibility and ease of development.
