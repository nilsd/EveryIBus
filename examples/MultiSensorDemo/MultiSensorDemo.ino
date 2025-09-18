/*
  MultiSensorDemo.ino - Simple multi-sensor telemetry example
  
  This example shows the super simple API for sending multiple
  sensor values to your FlySky FS-i6 transmitter.
  
  Hardware Setup:
  - Same as before: D0→SENS, D1→1kΩ→SENS, GND→GND
  
  Expected Result:
  - "IntV" (internal voltage)
  - "ExtV" (external voltage)  
  - "Temp" (temperature)
  - "RPM" (RPM)
  All appear on your FS-i6 transmitter with changing values!
*/

#include <EveryIBus.h>

// Create EveryIBus instance
EveryIBus ibus;

void setup() {
  // Initialize Serial for debug output (optional)
  Serial.begin(115200);
  
  // Super simple initialization!
  ibus.begin();  // Uses Serial1 by default
  
  // Optional: Enable debug output
  ibus.setDebug(true);
  
  Serial.println("EveryIBus Multi-Sensor Example");
  Serial.println("Check your FS-i6 for: IntV, ExtV, Temp, RPM");
}

void loop() {
  // IMPORTANT: Call update() regularly to handle iBUS protocol
  ibus.update();
  
  // Update all sensor values every second with realistic data
  static uint32_t lastUpdate = 0;
  if (millis() - lastUpdate > 1000) {
    lastUpdate = millis();
    
    // Simulate realistic sensor readings
    float time = millis() / 1000.0;
    
    // Battery voltages that change slowly
    float internalVolt = 5.0 + 0.1 * sin(time * 0.1);      // 4.9V to 5.1V
    float externalVolt = 12.0 + 0.5 * sin(time * 0.05);    // 11.5V to 12.5V
    
    // Temperature that varies
    float temperature = 20.0 + 5.0 * sin(time * 0.02);     // 15°C to 25°C
    
    // RPM that ramps up and down
    uint16_t rpm = 1000 + (uint16_t)(2000 * (sin(time * 0.03) + 1) / 2); // 1000-3000 RPM
    
    // Set all sensor values with the simple API
    ibus.setInternalVoltage(internalVolt);
    ibus.setExternalVoltage(externalVolt); 
    ibus.setTemperature(temperature);
    ibus.setRPM(rpm);
    
    // Debug output
    Serial.print("Set values - IntV: ");
    Serial.print(internalVolt, 2);
    Serial.print("V, ExtV: ");
    Serial.print(externalVolt, 2);
    Serial.print("V, Temp: ");
    Serial.print(temperature, 1);
    Serial.print("°C, RPM: ");
    Serial.println(rpm);
  }
  
  // Print statistics every 5 seconds
  static uint32_t lastStats = 0;
  if (millis() - lastStats > 5000) {
    lastStats = millis();
    
    Serial.print("Packets: ");
    Serial.print(ibus.getPacketCount());
    Serial.print(", Responses: ");
    Serial.print(ibus.getResponseCount());
    Serial.print(", Discovered: ");
    Serial.println(ibus.isDiscovered() ? "Yes" : "No");
  }
  
  // Your other application code can go here
  // The ibus.update() call is very fast (~0.1ms)
}