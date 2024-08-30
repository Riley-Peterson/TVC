#include <types.h>
#include <pin_defs.h>
#include <system_utils.h>
#include <flash_chip.h>
#include <sensors.h>
#include <BLE.h>
#include <controls.h>

bool chutes_fired = false;
unsigned long timeSinceLaunch;
unsigned long lastTime;

void setup() {
  Serial.begin(115200);
  delay(3000);
  initializePins();
  initializeBluetooth();
  initializeSensors();
  initializeControls();
  calibrateIMU();
  delay(500);
  while(!armed) {
    digitalWrite(led_pin, HIGH);
    delay(500);
    digitalWrite(led_pin, LOW);
    delay(500);
  }
  while(!launchDetect()) {
    Serial.println(getAccelMagnitude());
    delay(10);
  }
  delay(8000);
  onGround();
  timeSinceLaunch = millis();
  lastTime = timeSinceLaunch;
}

void loop() {
  if(lastTime - timeSinceLaunch > 4000 && !chutes_fired && lastTime - timeSinceLaunch < 7000) { // This will be done with an interrupt in the future
    digitalWrite(pyro_1, HIGH);
    chutes_fired = true;
  }
  if(lastTime - timeSinceLaunch > 7000 && chutes_fired) {// This will also be done with an interrupt in the future
    digitalWrite(pyro_1, LOW);
    chutes_fired = false;
  }

  updateControls();
  delay(10);
}