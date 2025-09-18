/*
  EveryIBus.h - iBUS telemetry library for Arduino Nano Every
  
  A lightweight, reliable iBUS telemetry implementation that works
  specifically with the Arduino Nano Every (ATmega4809) where the
  standard IBusBM library has timing issues.
  
  Hardware Setup:
  - D0 (RX) → direct connection to FS-iA6B SENS pin
  - D1 (TX) → 1kΩ resistor → FS-iA6B SENS pin  
  - GND → FS-iA6B GND
  
  Simple API:
  EveryIBus ibus;
  ibus.begin();
  ibus.setInternalVoltage(5.08);    // 5.08V
  ibus.setExternalVoltage(12.41);   // 12.41V  
  ibus.setTemperature(21.12);       // 21.12°C
  ibus.setRPM(4294);                // 4294 RPM
  
  Created for Arduino Nano Every
  Compatible with FlySky FS-iA6B receiver and FS-i6 transmitter
*/

#ifndef EVERYIBUS_H
#define EVERYIBUS_H

#include <Arduino.h>

// Internal sensor definitions (user doesn't need these)
#define IBUS_SENSOR_INTERNAL_VOLTAGE  0x00
#define IBUS_SENSOR_TEMPERATURE       0x01  
#define IBUS_SENSOR_RPM              0x02
#define IBUS_SENSOR_EXTERNAL_VOLTAGE  0x03

// iBUS protocol commands (internal use)
#define IBUS_CMD_DISCOVER            0x80
#define IBUS_CMD_TYPE                0x90
#define IBUS_CMD_MEASUREMENT         0xA0

// Maximum number of sensors we support
#define MAX_SENSORS 4

struct Sensor {
    uint8_t type;
    uint16_t value;
    bool hasValue;
};

class EveryIBus {
public:
    // Constructor
    EveryIBus();
    
    // Super simple initialization - uses Serial1 by default
    void begin(HardwareSerial& serial = Serial1);
    
    // Must be called regularly in loop() - handles protocol
    void update();
    
    // Simple sensor value setters - use real-world units
    void setInternalVoltage(float voltage);    // Volts (e.g., 5.08)
    void setExternalVoltage(float voltage);    // Volts (e.g., 12.41) 
    void setTemperature(float tempC);          // Celsius (e.g., 21.12)
    void setRPM(uint16_t rpm);                 // RPM (e.g., 4294)
    
    // Optional: Enable/disable debug output to Serial
    void setDebug(bool enable) { _debug = enable; }
    
    // Optional: Get statistics
    uint32_t getPacketCount() const { return _packetCount; }
    uint32_t getResponseCount() const { return _responseCount; }
    bool isDiscovered() const { return _anyDiscovered; }
    
private:
    HardwareSerial* _serial;
    Sensor _sensors[MAX_SENSORS];
    uint8_t _currentSensorIndex;
    bool _anyDiscovered;
    uint32_t _packetCount;
    uint32_t _responseCount;
    bool _debug;
    
    // Protocol handlers
    void handlePacket();
    void handleDiscoveryCommand(uint8_t address);
    void sendDiscoveryResponse(uint8_t address);
    void sendTypeResponse(uint8_t address);
    void sendMeasurementResponse(uint8_t address);
    
    // Utility functions
    bool validatePacket(uint8_t* packet);
    void sendPacket(uint8_t* data, uint8_t length);
    uint16_t calculateChecksum(uint8_t* data, uint8_t length);
    void clearSerialBuffer();
    void debugPrint(const char* message);
    void debugPrintHex(uint8_t* data, uint8_t length);
    
    // Helper functions
    void setSensorValue(uint8_t sensorType, uint16_t rawValue);
    int8_t findSensorIndex(uint8_t sensorType);
    uint8_t getNextAvailableAddress();
};

#endif // EVERYIBUS_H