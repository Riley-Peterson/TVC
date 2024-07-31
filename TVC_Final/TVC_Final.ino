#include <types.h>
#include <pin_defs.h>
#include <flash_chip.h>
#include <sensors.h>
#include <BLE.h>

void setup() {
  Serial.begin(115200);
  delay(3000);
  initializePins();
  initializeBluetooth();
  initializeSensors();
  calibrateIMU();
  delay(500);
}

void loop() {
  readSensors();
  getEuler();
  delay(100);
}
