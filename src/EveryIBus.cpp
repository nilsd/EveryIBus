/*
  EveryIBus.cpp - iBUS telemetry library for Arduino Nano Every
  
  Multi-sensor implementation with simple API for FlySky telemetry.
*/

#include "EveryIBus.h"

EveryIBus::EveryIBus() {
    _serial = nullptr;
    _currentSensorIndex = 0;
    _anyDiscovered = false;
    _packetCount = 0;
    _responseCount = 0;
    _debug = false;
    
    // Initialize all sensors as unused
    for (int i = 0; i < MAX_SENSORS; i++) {
        _sensors[i].type = 0xFF;  // Invalid type
        _sensors[i].value = 0;
        _sensors[i].hasValue = false;
    }
}

void EveryIBus::begin(HardwareSerial& serial) {
    _serial = &serial;
    
    // Initialize serial at iBUS standard baud rate
    _serial->begin(115200);
    
    // Clear any initial garbage
    delay(100);
    clearSerialBuffer();
    
    if (_debug) {
        Serial.println(F("EveryIBus: Multi-sensor mode initialized"));
    }
}

void EveryIBus::update() {
    if (!_serial) return;
    
    // Check for incoming packets
    if (_serial->available() >= 4) {
        handlePacket();
    }
}

// Simple API functions - convert real-world units to iBUS format
void EveryIBus::setInternalVoltage(float voltage) {
    uint16_t rawValue = (uint16_t)(voltage * 100.0f); // Convert to 0.01V units
    setSensorValue(IBUS_SENSOR_INTERNAL_VOLTAGE, rawValue);
}

void EveryIBus::setExternalVoltage(float voltage) {
    uint16_t rawValue = (uint16_t)(voltage * 100.0f); // Convert to 0.01V units
    setSensorValue(IBUS_SENSOR_EXTERNAL_VOLTAGE, rawValue);
}

void EveryIBus::setTemperature(float tempC) {
    // iBUS temperature: 0.1°C units where 0 = -40°C
    // So 21.12°C = (21.12 + 40) * 10 = 611.2 ≈ 611
    uint16_t rawValue = (uint16_t)((tempC + 40.0f) * 10.0f);
    setSensorValue(IBUS_SENSOR_TEMPERATURE, rawValue);
}

void EveryIBus::setRPM(uint16_t rpm) {
    setSensorValue(IBUS_SENSOR_RPM, rpm);
}

void EveryIBus::setSensorValue(uint8_t sensorType, uint16_t rawValue) {
    // Find existing sensor or create new one
    int8_t index = findSensorIndex(sensorType);
    
    if (index == -1) {
        // Find empty slot
        for (int i = 0; i < MAX_SENSORS; i++) {
            if (!_sensors[i].hasValue) {
                _sensors[i].type = sensorType;
                _sensors[i].value = rawValue;
                _sensors[i].hasValue = true;
                
                if (_debug) {
                    Serial.print(F("EveryIBus: Added sensor type "));
                    Serial.print(sensorType);
                    Serial.print(F(" at index "));
                    Serial.println(i);
                }
                return;
            }
        }
        
        if (_debug) {
            Serial.println(F("EveryIBus: Warning - No free sensor slots"));
        }
    } else {
        // Update existing sensor
        _sensors[index].value = rawValue;
    }
}

int8_t EveryIBus::findSensorIndex(uint8_t sensorType) {
    for (int i = 0; i < MAX_SENSORS; i++) {
        if (_sensors[i].hasValue && _sensors[i].type == sensorType) {
            return i;
        }
    }
    return -1; // Not found
}

uint8_t EveryIBus::getNextAvailableAddress() {
    // Return address 1-4 based on how many sensors we have
    uint8_t count = 0;
    for (int i = 0; i < MAX_SENSORS; i++) {
        if (_sensors[i].hasValue) {
            count++;
        }
    }
    return count; // This will be 1-4 for the next sensor to be discovered
}

void EveryIBus::handlePacket() {
    uint8_t packet[4];
    
    // Read 4-byte packet
    for (int i = 0; i < 4; i++) {
        packet[i] = _serial->read();
    }
    
    _packetCount++;
    
    if (_debug) {
        debugPrint("RX: ");
        debugPrintHex(packet, 4);
    }
    
    // Validate packet structure and checksum
    if (validatePacket(packet)) {
        uint8_t command = packet[1] & 0xF0;
        uint8_t address = packet[1] & 0x0F;
        
        // Handle addresses 1-4 for our sensors
        if (address >= 1 && address <= 4) {
            switch (command) {
                case IBUS_CMD_DISCOVER:
                    handleDiscoveryCommand(address);
                    break;
                    
                case IBUS_CMD_TYPE:
                    sendTypeResponse(address);
                    break;
                    
                case IBUS_CMD_MEASUREMENT:
                    sendMeasurementResponse(address);
                    break;
            }
        }
    }
    
    if (_debug) {
        Serial.println();
    }
    
    // Clear buffer AFTER processing
    clearSerialBuffer();
}

void EveryIBus::handleDiscoveryCommand(uint8_t address) {
    // Check if we have a sensor for this address
    if (address <= MAX_SENSORS && _sensors[address - 1].hasValue) {
        sendDiscoveryResponse(address);
        _anyDiscovered = true;
        
        if (_debug) {
            debugPrint(" -> DISCOVERY ADDR:");
            Serial.print(address);
            debugPrint(" [SENT]");
        }
    } else {
        if (_debug) {
            debugPrint(" -> DISCOVERY ADDR:");
            Serial.print(address);
            debugPrint(" (no sensor)");
        }
    }
}

void EveryIBus::sendDiscoveryResponse(uint8_t address) {
    // Echo back the discovery packet exactly
    uint8_t response[4];
    response[0] = 0x04;
    response[1] = 0x80 | address;  // Command + address
    
    // Calculate checksum
    uint16_t checksum = 0xFFFF - (response[0] + response[1]);
    response[2] = checksum & 0xFF;
    response[3] = (checksum >> 8) & 0xFF;
    
    sendPacket(response, 4);
    _responseCount++;
}

void EveryIBus::sendTypeResponse(uint8_t address) {
    if (address > MAX_SENSORS || !_sensors[address - 1].hasValue) return;
    
    uint8_t response[6];
    response[0] = 0x06;  // Packet length
    response[1] = 0x90 | address;  // Command + address
    response[2] = _sensors[address - 1].type;  // Sensor type
    response[3] = 0x02;  // Always 0x02
    
    // Calculate checksum
    uint16_t checksum = calculateChecksum(response, 4);
    response[4] = checksum & 0xFF;
    response[5] = (checksum >> 8) & 0xFF;
    
    sendPacket(response, 6);
    _responseCount++;
    
    if (_debug) {
        debugPrint(" -> TYPE ADDR:");
        Serial.print(address);
        debugPrint(" [SENT]");
    }
}

void EveryIBus::sendMeasurementResponse(uint8_t address) {
    if (address > MAX_SENSORS || !_sensors[address - 1].hasValue) return;
    
    uint8_t response[6];
    response[0] = 0x06;  // Packet length
    response[1] = 0xA0 | address;  // Command + address
    
    uint16_t value = _sensors[address - 1].value;
    response[2] = value & 0xFF;        // Value low byte
    response[3] = (value >> 8) & 0xFF; // Value high byte
    
    // Calculate checksum
    uint16_t checksum = calculateChecksum(response, 4);
    response[4] = checksum & 0xFF;
    response[5] = (checksum >> 8) & 0xFF;
    
    sendPacket(response, 6);
    _responseCount++;
    
    if (_debug) {
        debugPrint(" -> MEASUREMENT ADDR:");
        Serial.print(address);
        debugPrint(" [SENT]");
    }
}

// ... [Keep all the existing utility functions unchanged]

bool EveryIBus::validatePacket(uint8_t* packet) {
    // Check packet length
    if (packet[0] != 0x04) {
        return false;
    }
    
    // Verify checksum
    uint16_t expectedChecksum = 0xFFFF - (packet[0] + packet[1]);
    uint16_t receivedChecksum = (packet[3] << 8) | packet[2];
    
    return (expectedChecksum == receivedChecksum);
}

void EveryIBus::sendPacket(uint8_t* data, uint8_t length) {
    if (!_serial) return;
    
    // Send immediately - no delays for timing-critical iBUS protocol
    for (uint8_t i = 0; i < length; i++) {
        _serial->write(data[i]);
    }
    
    // Ensure transmission completes
    _serial->flush();
}

uint16_t EveryIBus::calculateChecksum(uint8_t* data, uint8_t length) {
    uint16_t sum = 0;
    for (uint8_t i = 0; i < length; i++) {
        sum += data[i];
    }
    return 0xFFFF - sum;
}

void EveryIBus::clearSerialBuffer() {
    while (_serial && _serial->available()) {
        _serial->read();
    }
}

void EveryIBus::debugPrint(const char* message) {
    if (_debug) {
        Serial.print(message);
    }
}

void EveryIBus::debugPrintHex(uint8_t* data, uint8_t length) {
    if (_debug) {
        for (uint8_t i = 0; i < length; i++) {
            if (data[i] < 0x10) Serial.print(F("0"));
            Serial.print(data[i], HEX);
            if (i < length - 1) Serial.print(F(" "));
        }
    }
}