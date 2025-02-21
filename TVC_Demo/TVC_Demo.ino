#include <types.h>
#include <pin_defs.h>
#include <system_utils.h>
#include <sensors.h>
#include <BLE.h>
#include <controls.h>
#include <flash_chip.h>

bool chutes_fired = false;
unsigned long launch_start_time;
unsigned long time_since_launch;

void setup() {
  Serial.begin(115200);
  delay(3000);
  initializePins();
  initializeBluetooth();
  initializeSensors();
  initializeControls();
  calibrateIMU();
  delay(10);
  readSensors();

  ground_alt = altitude;
  delay(500);

  //while(!armed) {
  //  blinkFast(1);
  //}
}

void loop() {
  Serial.println((time_since_launch - launch_start_time) / 1000);
  readSensors();
  updateEuler();
  if(demoMode == 0) {
    moveGimbalInCircle();
  }
  if(demoMode == 2) {
    updateFirstControls();
    delay(50);
  }
  time_since_launch = millis();
  //delay(50);
}