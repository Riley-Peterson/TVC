#ifndef SENSORS_H
#define SENSORS_H

#include <Wire.h>
#include <BMI085.h>
#include <imuFilter.h>
#include <MS5611.h>

// Objects
BMI085Accel accel(Wire, 0x18);
BMI085Gyro gyro(Wire, 0x68);
MS5611 baro(0x77);
imuFilter fusion;

// Variables
quat_t initial_quat = {0, 0, 0, 0}; // Initial quaternion
quat_t initial_quat_conjugate = {0, 0, 0, 0}; // Conjugate of the initial quaternion
quat_t q = {0, 0, 0, 0};
int astatus;
float pitch = 0;
float yaw = 0;
float roll = 0;
float pressure = 0;
float temperature = 0;
float altitude = 0;
float max_alt = 0;
float ground_alt = 0;


float pressure_to_altitude(float press) {
	return 145366.45 * (1 - pow((press / 1013.25), 0.190284));
}

inline void initializeSensors() { // Initialize Sensors
  Wire.begin(SDA0_Pin, SCL0_Pin);
  // Initialize accelerometer
  astatus = accel.begin();
  if (astatus < 0) {
    Serial.println("Accel Initialization Error");
    Serial.println(astatus);
    while (1) {
      blinkSlow(1);
    }
  } else {
    Serial.println("Accelerometer initialized successfully");
  }
  // Initialize gyroscope
  astatus = gyro.begin();
  if (astatus < 0) {
    Serial.println("Gyro Initialization Error");
    Serial.println(astatus);
    while (1) {
      blinkSlow(1);
    }
  } else {
    Serial.println("Gyroscope initialized successfully");
  }
  if(baro.begin() == true) {
    Serial.println("Barometer initialized succesfully");
  } else {
    Serial.println("Barometer Initialization Error");
    while(1) {
      blinkSlow(1);
    }
  }

  fusion.setup(accel.getAccelX_mss(), accel.getAccelY_mss(), accel.getAccelZ_mss());
}

inline void readSensors() { // Read sensor values and store quaternion output
  gyro.readSensor();
  accel.readSensor();
  baro.read();
  vec3_t gyro_rads(gyro.getGyroX_rads(), gyro.getGyroY_rads(), gyro.getGyroZ_rads());
  vec3_t curr_accel(accel.getAccelX_mss(), accel.getAccelY_mss(), accel.getAccelZ_mss());
  pressure = baro.getPressure();
  temperature = baro.getTemperature();
  altitude = pressure_to_altitude(pressure);
  
  if(altitude > max_alt) {
    max_alt = altitude;
  }

  //Serial.println(altitude);
  //Serial.printf("x:%.2f,y:%.2f,z:%.2f", curr_accel.x, curr_accel.y, curr_accel.z);
  //Serial.println();

  fusion.update(gyro_rads.x, gyro_rads.y, gyro_rads.z, curr_accel.x, curr_accel.y, curr_accel.z);

  q = fusion.getQuat();
  q = initial_quat_conjugate * q;
}

inline void calibrateIMU() { // Zero out sensor
  gyro.readSensor();
  accel.readSensor();
  vec3_t gyro_rads(gyro.getGyroX_rads(), gyro.getGyroY_rads(), gyro.getGyroZ_rads());
  vec3_t curr_accel(accel.getAccelX_mss(), accel.getAccelY_mss(), accel.getAccelZ_mss());
  fusion.update(gyro_rads.x, gyro_rads.y, gyro_rads.z, curr_accel.x, curr_accel.y, curr_accel.z);
  // Capture the initial quaternion
  initial_quat = fusion.getQuat();
  // Compute its conjugate
  initial_quat_conjugate = {initial_quat.w, -initial_quat.v.x, -initial_quat.v.y, -initial_quat.v.z};
}

inline void updateEuler() { // Convert quats to euler angles
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
}

inline void printEuler() {
  Serial.printf("roll:%.2f,pitch:%.2f,yaw:%.2f", roll, pitch, yaw);
  Serial.println();
}

inline void printQuat() {
  Serial.printf("w:%.2f,x:%.2f,y:%.2f,z:%.2f", q.w, q.v.x, q.v.y, q.v.z);
  Serial.println();
}

float getAccelMagnitude() {
  accel.readSensor();
  return abs(accel.getAccelX_mss()) + abs(accel.getAccelY_mss()) + abs(accel.getAccelZ_mss());
}

#endif