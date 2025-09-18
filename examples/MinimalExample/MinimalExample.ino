#include <EveryIBus.h>

EveryIBus ibus;

void setup() {
  ibus.begin();
}

void loop() {
  ibus.update();
  
  ibus.setInternalVoltage(5.08);    // 5.08V
  ibus.setExternalVoltage(12.41);   // 12.41V  
  ibus.setTemperature(21.12);       // 21.12°C
  ibus.setRPM(4294);                // 4294 RPM
}