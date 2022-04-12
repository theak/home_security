# home_security
Use an ESP32 Arduino to interface with a traditional home security system (Honeywell/ADT)

Monitors Zone statuses via ADC ports and communicates any change in state via MQTT. This can be consumed anywhere, e.g. from Home Assistant.

To get started:
1. Copy config.h.template to config.h
2. Edit the values to match your setup
3. Open main/main.ino in Arduino IDE
4. Install the PubSubClient library (Sketch -> Include libraries -> Manage libraries)
5. Select the device type, compile, and run. If necessary, install support for the relevant device (e.g. ESP32).
