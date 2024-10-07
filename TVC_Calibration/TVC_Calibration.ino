#include <types.h>
#include <pin_defs.h>
#include <system_utils.h>
#include <sensors.h>
#include <BLE.h>
#include <controls.h>
#include <flash_chip.h>

unsigned long launch_start_time;
unsigned long time_since_launch;
//bool 

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

  servoX.write(90);
  servoY.write(90);
  while(!armed) {
    blinkFast(1);  
  }

  launch_start_time = millis();
  time_since_launch = launch_start_time;
}

void loop() {
  Serial.println((time_since_launch - launch_start_time) / 1000);
  readSensors();
  updateEuler();
  
  servoX.write(tempX);
  servoY.write(tempY);
  //updateFirstControls();
  delay(10);
}