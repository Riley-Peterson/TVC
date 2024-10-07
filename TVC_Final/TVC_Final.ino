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
  initializeFlash();
  calibrateIMU();
  delay(10);
  readSensors();

  ground_alt = altitude;

  if(digitalRead(select) == LOW) {
    Serial.println("Holding");
    dumpData();
    onGround();
  }
  delay(500);

  Serial.println("Erasing Flash... ");
  flash.eraseChip();
  Serial.println("Flash Erased");
  erasedFlashSound();
  servoX.write(90);
  servoY.write(90);
  while(!armed) {
    blinkFast(1);
  }

  //curr_phase = FlightPhase::PAD_IDLE;
  curr_phase = "PAD_IDLE";

  float tempAccelMag = getAccelMagnitude();
  while(!launchDetect()) {
    readSensors();
    updateEuler();
    if(getAccelMagnitude() < tempAccelMag + 1 && getAccelMagnitude() > tempAccelMag - 1) {
      calibrateIMU();
    }
    //Serial.println(getAccelMagnitude());
    updateControls();
    delay(10);
  }

  //curr_phase = FlightPhase::ASCENT;
  curr_phase = "ASCENT";

  launch_start_time = millis();
  time_since_launch = launch_start_time;
}

void loop() {
  Serial.println((time_since_launch - launch_start_time) / 1000);
  readSensors();
  updateEuler();
  if(max_alt > altitude + 2 && curr_phase == "ASCENT") {
    curr_phase = "APOGEE";
    //curr_phase = FlightPhase::APOGEE;
  }
  if(time_since_launch - launch_start_time > 4000 && !chutes_fired && time_since_launch - launch_start_time < 7000) { // This will be done with an interrupt in the future
    digitalWrite(pyro_1, HIGH);
    chutes_fired = true;
    curr_phase = "MAIN";
    //curr_phase = FlightPhase::MAIN;
  }
  if(time_since_launch - launch_start_time > 7000 && chutes_fired) { // This will also be done with an interrupt in the future
    digitalWrite(pyro_1, LOW);
    chutes_fired = false;
  }
  if(getAccelMagnitude() > 9.0 && getAccelMagnitude() < 15.0 && time_since_launch - launch_start_time > 15000) {
    curr_phase = "GROUND";
    //curr_phase = FlightPhase::GROUND;
    onGround();
  }
  
  updateControls();
  /*
  data.time = time_since_launch - launch_start_time;
  data.temperature = temperature;
  data.pressure = pressure;
  data.altitude = altitude - ground_alt;
  data.orientation = q;
  vec3_t curr_accel(accel.getAccelX_mss(), accel.getAccelY_mss(), accel.getAccelZ_mss());
  data.acceleration = curr_accel;
  data.servoActuationX = actuation_x;
  data.servoActuationY = actuation_y;
  data.phase = curr_phase;
  writeData(data);
  */
  // Band Aid Solution
  writeData(String(time_since_launch - launch_start_time));
  writeData(String(temperature));
  writeData(String(pressure));
  writeData(String(altitude - ground_alt));
  writeData(String(pitch));
  writeData(String(yaw));
  writeData(String(roll));
  writeData(String(accel.getAccelX_mss()));
  writeData(String(accel.getAccelY_mss()));
  writeData(String(accel.getAccelZ_mss()));
  writeData(String(actuation_x));
  writeData(String(actuation_y));
  writeData(curr_phase);
  time_since_launch = millis();
  delay(10);
}