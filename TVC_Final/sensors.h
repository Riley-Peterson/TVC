#ifndef SENSORS_H
#define SENSORS_H

#include <Wire.h>
#include <BMI085.h>
#include <imuFilter.h>

// IMU objects
BMI085Accel accel(Wire, 0x18);
BMI085Gyro gyro(Wire, 0x68);
imuFilter fusion;
float ALPHA = 0.1; // High-pass filter coefficient
quat_t initial_quat = {0, 0, 0, 0}; // Initial quaternion
quat_t initial_quat_conjugate = {0, 0, 0, 0}; // Conjugate of the initial quaternion
int astatus;

inline void initializeSensors() {
  Wire.begin(SDA0_Pin, SCL0_Pin);
  // Initialize accelerometer
  astatus = accel.begin();
  if (astatus < 0) {
    Serial.println("Accel Initialization Error");
    Serial.println(astatus);
    digitalWrite(led_pin, HIGH);
    delay(1000);
    digitalWrite(led_pin, LOW);
    delay(1000);
    digitalWrite(led_pin, HIGH);
    while (1) {}
  } else {
    Serial.println("Accelerometer initialized successfully");
  }
  // Initialize gyroscope
  astatus = gyro.begin();
  if (astatus < 0) {
    Serial.println("Gyro Initialization Error");
    Serial.println(astatus);
    digitalWrite(led_pin, HIGH);
    delay(1000);
    digitalWrite(led_pin, LOW);
    delay(1000);
    digitalWrite(led_pin, HIGH);
    while (1) {}
  } else {
    Serial.println("Gyroscope initialized successfully");
  }

  fusion.setup(accel.getAccelX_mss(), accel.getAccelY_mss(), accel.getAccelZ_mss());
}

#endif