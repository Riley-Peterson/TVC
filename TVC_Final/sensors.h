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
quat_t q = {0, 0, 0, 0};
int astatus;
float pitch = 0;
float yaw = 0;
float roll = 0;

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

inline void readSensors() {
  gyro.readSensor();
  accel.readSensor();
  vec3_t gyro_rads(gyro.getGyroX_rads(), gyro.getGyroY_rads(), gyro.getGyroZ_rads());
  vec3_t curr_accel(accel.getAccelX_mss(), accel.getAccelY_mss(), accel.getAccelZ_mss());
  vec3_t filt_accel = ALPHA * curr_accel;
  fusion.update(gyro_rads.x, gyro_rads.y, gyro_rads.z, filt_accel.x, filt_accel.y, filt_accel.z);

  q = fusion.getQuat();
  q = initial_quat_conjugate * q;

  Serial.printf("w:%.2f,x:%.2f,y:%.2f,z:%.2f", q.w, q.v.x, q.v.y, q.v.z);
  Serial.println();
}

inline void calibrateIMU() {
  gyro.readSensor();
  accel.readSensor();
  vec3_t gyro_rads(gyro.getGyroX_rads(), gyro.getGyroY_rads(), gyro.getGyroZ_rads());
  vec3_t curr_accel(accel.getAccelX_mss(), accel.getAccelY_mss(), accel.getAccelZ_mss());
  vec3_t filt_accel = ALPHA * curr_accel;
  fusion.update(gyro_rads.x, gyro_rads.y, gyro_rads.z, filt_accel.x, filt_accel.y, filt_accel.z);
  // Capture the initial quaternion
  initial_quat = fusion.getQuat();
  // Compute its conjugate
  initial_quat_conjugate = {initial_quat.w, -initial_quat.v.x, -initial_quat.v.y, -initial_quat.v.z};
}

inline void getEuler() {
  float sinr_cosp = 2 * (q.w * q.v.x + q.v.y * q.v.z);
  float cosr_cosp = 1 - 2 * (q.v.x * q.v.x + q.v.y * q.v.y);
  roll = (atan2(sinr_cosp, cosr_cosp)) * (180 / PI);

  float sinp = 2 * (q.w * q.v.y - q.v.z * q.v.x);
  pitch;
  if (abs(sinp) >= 1) {
    pitch = (copysign(PI / 2, sinp)) * (180 / PI);
  } else {
    pitch = (asin(sinp)) * (180 / PI);
  }

  float siny_cosp = 2 * (q.w * q.v.z + q.v.x * q.v.y);
  float cosy_cosp = 1 - 2 * (q.v.y * q.v.y + q.v.z * q.v.z);
  yaw = (atan2(siny_cosp, cosy_cosp)) * (180 / PI);

  Serial.printf("roll:%.2f,pitch:%.2f,yaw:%.2f", roll, pitch, yaw);
  Serial.println();
}

#endif