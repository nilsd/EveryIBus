# EveryIBus

A lightweight, reliable iBUS telemetry library specifically designed for Arduino Nano Every (ATmega4809).

## Why EveryIBus?

The popular IBusBM library has timing issues on Arduino Nano Every due to differences in the ATmega4809's timer architecture compared to the ATmega328P. EveryIBus solves these problems with a direct, interrupt-friendly implementation that works reliably on the Nano Every.

## Features

- ✅ **Reliable on Nano Every** - No timer interrupt conflicts
- ✅ **Simple API** - Easy to integrate into existing projects  
- ✅ **Interrupt-friendly** - Works alongside pin interrupts and other time-sensitive code
- ✅ **Multiple sensor types** - Voltage, temperature, current, RPM, etc.
- ✅ **Debug support** - Built-in diagnostic output
- ✅ **Lightweight** - Minimal memory footprint

## Hardware Setup

```
Arduino Nano Every        FlySky FS-iA6B
┌─────────────────┐       ┌──────────────┐
│              D0 │──────→│ SENS (RX)    │
│              D1 │──1kΩ─→│ SENS (TX)    │  
│             GND │──────→│ GND          │
└─────────────────┘       └──────────────┘
```

**Important:** Use a 1kΩ resistor on the TX line to prevent damage.

## Quick Start

```cpp
#include <EveryIBus.h>

EveryIBus ibus;

void setup() {
  // Initialize iBUS on Serial1 as voltage sensor
  ibus.begin(Serial1, IBUS_SENSOR_INTERNAL_VOLTAGE);
}

void loop() {
  // Handle iBUS protocol (call regularly!)
  ibus.update();
  
  // Update sensor value
  ibus.setSensorValue(1234); // 12.34V
  
  // Your other code here...
}
```

## Sensor Types

| Constant | Description | Units |
|----------|-------------|-------|
| `IBUS_SENSOR_INTERNAL_VOLTAGE` | Internal voltage | 0.01V (1234 = 12.34V) |
| `IBUS_SENSOR_EXTERNAL_VOLTAGE` | External voltage | 0.01V |
| `IBUS_SENSOR_TEMPERATURE` | Temperature | 0.1°C (0 = -40°C) |
| `IBUS_SENSOR_CURRENT` | Current | 0.01A |
| `IBUS_SENSOR_RPM` | RPM | 1 RPM |
| `IBUS_SENSOR_FUEL` | Fuel percentage | 1% (0-100) |

## API Reference

### `EveryIBus()`
Constructor. Creates an EveryIBus instance.

### `void begin(HardwareSerial& serial, uint8_t sensorType)`
Initialize the library.
- `serial`: Hardware serial port (use `Serial1` for D0/D1)
- `sensorType`: One of the `IBUS_SENSOR_*` constants

### `void update()`
Handle iBUS protocol. **Must be called regularly in loop().**

### `void setSensorValue(uint16_t value)`
Set the sensor value in appropriate units for the sensor type.

### `bool isDiscovered()`
Returns true if the sensor has been discovered by the receiver.

### `uint32_t getPacketCount()`
Returns the number of packets received from the receiver.

### `uint32_t getResponseCount()`
Returns the number of responses sent to the receiver.

### `void setDebug(bool enable)`
Enable/disable debug output to Serial monitor.

## Examples

See the `examples/` folder for:
- **BasicTelemetry** - Simple voltage telemetry
- **WithInterrupts** - Using iBUS alongside pin interrupts
- **INA260Example** - Real sensor integration (coming soon)

## Compatibility

- ✅ Arduino Nano Every (ATmega4809)
- ✅ FlySky FS-iA6B receiver
- ✅ FlySky FS-i6/FS-i6X transmitter
- ❓ Other megaAVR boards (untested)

## Troubleshooting

**No telemetry on transmitter:**
1. Check wiring (especially the 1kΩ resistor on TX)
2. Verify GND connection
3. Enable debug: `ibus.setDebug(true)`
4. Check Serial monitor for "DISCOVERY [SENT]" messages

**Irregular telemetry:**
1. Ensure `ibus.update()` is called frequently in `loop()`
2. Avoid long delays in main loop
3. Check for interrupt conflicts

**Compile errors:**
1. Ensure library is in `libraries/every-ibus/` folder
2. Restart Arduino IDE
3. Select "Arduino Nano Every" as board

## License

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.

## Contributing

Issues and pull requests welcome! Please test thoroughly on Arduino Nano Every hardware.