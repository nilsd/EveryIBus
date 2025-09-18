# EveryIBus

A lightweight, reliable iBUS telemetry library specifically designed for **Arduino Nano Every** (ATmega4809).

## 🎯 Why EveryIBus?

The popular IBusBM library has timing issues on Arduino Nano Every due to differences in the ATmega4809's timer architecture. EveryIBus solves these problems with a direct, interrupt-friendly implementation that **just works** on the Nano Every.

## ✨ Features

- ✅ **Reliable on Nano Every** - No timer interrupt conflicts
- ✅ **Super Simple API** - Four-line setup for multiple sensors
- ✅ **Real-world units** - Use volts, celsius, RPM directly  
- ✅ **Interrupt-friendly** - Works alongside pin interrupts and other time-sensitive code
- ✅ **Multiple sensors** - Up to 4 sensors automatically managed
- ✅ **Zero configuration** - Works out-of-the-box with sensible defaults
- ✅ **Lightweight** - Minimal memory footprint
- ✅ **Debug support** - Built-in diagnostic output

## 🚀 Quick Start

```cpp
#include <EveryIBus.h>

EveryIBus ibus;

void setup() {
  ibus.begin();  // That's it!
}

void loop() {
  ibus.update();  // Handle protocol
  
  // Set sensor values with real-world units
  ibus.setInternalVoltage(5.08);    // 5.08V
  ibus.setExternalVoltage(12.41);   // 12.41V  
  ibus.setTemperature(21.12);       // 21.12°C
  ibus.setRPM(4294);                // 4294 RPM
}
```

That's it! Your FS-i6 transmitter will show **IntV**, **ExtV**, **Temp**, and **RPM** with the values you set.

## 🔌 Hardware Setup

```
Arduino Nano Every        FlySky FS-iA6B
┌─────────────────┐       ┌──────────────┐
│              D0 │──────→│ SENS (RX)    │
│              D1 │──1kΩ─→│ SENS (TX)    │  
│             GND │──────→│ GND          │
└─────────────────┘       └──────────────┘
```

**Important:** Use a 1kΩ resistor on the TX line (D1) to prevent damage.

## 🎮 Compatible Hardware

- ✅ **Arduino Nano Every** (ATmega4809) - Primary target
- ✅ **FlySky FS-iA6B** receiver - Tested and confirmed
- ✅ **FlySky FS-i6/FS-i6X** transmitter - Shows all sensor types
- ❓ Other megaAVR boards - Should work but untested

## 📊 Supported Sensors

| Sensor Type | Function | Units | Transmitter Display |
|-------------|----------|-------|-------------------|
| Internal Voltage | `setInternalVoltage(5.08)` | Volts | IntV: 5.08V |
| External Voltage | `setExternalVoltage(12.41)` | Volts | ExtV: 12.41V |
| Temperature | `setTemperature(21.12)` | Celsius | Temp: 21.1°C |
| RPM | `setRPM(4294)` | RPM | RPM: 4294 |

## 📚 Examples

### Basic Usage
```cpp
#include <EveryIBus.h>

EveryIBus ibus;

void setup() {
  ibus.begin();
}

void loop() {
  ibus.update();
  
  // Your sensor readings here
  float batteryVoltage = analogRead(A0) * (5.0 / 1023.0);
  ibus.setInternalVoltage(batteryVoltage);
}
```

### With Pin Interrupts
```cpp
#include <EveryIBus.h>

EveryIBus ibus;
volatile int rpmCounter = 0;

void setup() {
  ibus.begin();
  attachInterrupt(digitalPinToInterrupt(2), rpmISR, RISING);
}

void loop() {
  ibus.update();  // Non-blocking, interrupt-safe
  
  // Calculate RPM from interrupt counter
  static uint32_t lastRpmCalc = 0;
  if (millis() - lastRpmCalc > 1000) {
    uint16_t rpm = rpmCounter * 60;  // Convert to RPM
    ibus.setRPM(rpm);
    rpmCounter = 0;
    lastRpmCalc = millis();
  }
}

void rpmISR() {
  rpmCounter++;
}
```

### Real Sensor Integration (INA260)
```cpp
#include <EveryIBus.h>
#include <Adafruit_INA260.h>

EveryIBus ibus;
Adafruit_INA260 ina260 = Adafruit_INA260();

void setup() {
  ibus.begin();
  ina260.begin();
}

void loop() {
  ibus.update();
  
  // Read real sensor data
  float voltage = ina260.readBusVoltage();
  float current = ina260.readCurrent();
  
  // Send to transmitter
  ibus.setExternalVoltage(voltage);
  // Convert current to another sensor type if needed
}
```

## 🛠️ Installation

### Arduino Library Manager (Recommended)
1. Open Arduino IDE
2. Go to **Sketch → Include Library → Manage Libraries**
3. Search for "EveryIBus"
4. Click **Install**

### Manual Installation
1. Download the latest release from [Releases](../../releases)
2. Extract to `Arduino/libraries/EveryIBus/`
3. Restart Arduino IDE

### Development Version
```bash
cd ~/Arduino/libraries/
git clone https://github.com/yourusername/EveryIBus.git
```

## 🐛 Troubleshooting

**No telemetry on transmitter:**
1. Check wiring (especially the 1kΩ resistor on TX)
2. Verify ground connection between Arduino and receiver
3. Enable debug: `ibus.setDebug(true)` and check Serial Monitor
4. Look for "DISCOVERY [SENT]" messages in debug output

**Irregular telemetry:**
1. Ensure `ibus.update()` is called frequently in `loop()`
2. Avoid long `delay()` calls in main loop
3. Check for interrupt conflicts with other libraries

**Compilation errors:**
1. Ensure you selected "Arduino Nano Every" as the board
2. Verify library is installed correctly
3. Restart Arduino IDE

## 🤝 Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Test thoroughly on Arduino Nano Every hardware
4. Commit your changes (`git commit -m 'Add amazing feature'`)
5. Push to the branch (`git push origin feature/amazing-feature`)
6. Open a Pull Request

## 📄 License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **Betaflight project** for iBUS protocol documentation
- **FlySky community** for protocol reverse engineering  
- **Arduino community** for continuous support
- **IBusBM library** authors for inspiration (even though it doesn't work on Nano Every 😅)

## 📞 Support

- 🐛 **Bug reports:** [GitHub Issues](../../issues)
- 💡 **Feature requests:** [GitHub Issues](../../issues)
- 📖 **Documentation:** [Wiki](../../wiki)
- 💬 **Discussions:** [GitHub Discussions](../../discussions)

---

**Made with ❤️ for the Arduino Nano Every community**

*Finally, iBUS telemetry that just works!*