#include <quaternion_type.h>
#include <vector_type.h>
#include <SPI.h>
#include <SPIMemory.h>
#include <pin_defs.h>
#include <types.h>
#include <sensors.h>
#include <BLE.h>

SPIFlash flash_5(chip_select);

void setup() {
  Serial.begin(115200);
  initializePins();
  initializeBluetooth();
  initializeSensors();
  delay(3000);
  delay(100);
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

void loop() {
  gyro.readSensor();
  accel.readSensor();
  delay(100);
  vec3_t gyro_rads(gyro.getGyroX_rads(), gyro.getGyroY_rads(), gyro.getGyroZ_rads());
  vec3_t curr_accel(accel.getAccelX_mss(), accel.getAccelY_mss(), accel.getAccelZ_mss());
  vec3_t filt_accel = ALPHA * curr_accel;
  fusion.update(gyro_rads.x, gyro_rads.y, gyro_rads.z, filt_accel.x, filt_accel.y, filt_accel.z);
  
  // Get the current quaternion
  quat_t q = fusion.getQuat();
  
  // Correct the quaternion by multiplying with the initial conjugate
  q = initial_quat_conjugate * q;
  
  // Convert the corrected quaternion to Euler angles
  float sinr_cosp = 2 * (q.w * q.v.x + q.v.y * q.v.z);
  float cosr_cosp = 1 - 2 * (q.v.x * q.v.x + q.v.y * q.v.y);
  float roll = (atan2(sinr_cosp, cosr_cosp)) * (180 / PI);

  float sinp = 2 * (q.w * q.v.y - q.v.z * q.v.x);
  float pitch;
  if (abs(sinp) >= 1) {
    pitch = (copysign(PI / 2, sinp)) * (180 / PI);
  } else {
    pitch = (asin(sinp)) * (180 / PI);
  }

  float siny_cosp = 2 * (q.w * q.v.z + q.v.x * q.v.y);
  float cosy_cosp = 1 - 2 * (q.v.y * q.v.y + q.v.z * q.v.z);
  float yaw = (atan2(siny_cosp, cosy_cosp)) * (180 / PI);

  Serial.printf("roll:%.2f,pitch:%.2f,yaw:%.2f", roll, pitch, yaw);
  //Serial.printf("w:%.2f,x:%.2f,y:%.2f,z:%.2f", q.w, q.v.x, q.v.y, q.v.z);
  Serial.println();
}