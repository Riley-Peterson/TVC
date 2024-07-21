#include <Wire.h>
#include <BMI085.h>
#include <imuFilter.h>
#include <quaternion_type.h>
#include <vector_type.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <SPI.h>
#include <SPIMemory.h>
#include <pin_defs.h>
#include <sensor.h>
#include <types.h>
//#include <BLE.h>

// IMU objects
BMI085Accel accel(Wire, 0x18);
BMI085Gyro gyro(Wire, 0x68);

SPIFlash flash_5(CS);
imuFilter fusion;
float ALPHA = 0.1; // High-pass filter coefficient
quat_t initialQuat = {0, 0, 0, 0}; // Initial quaternion
quat_t initialQuatConjugate = {0, 0, 0, 0}; // Conjugate of the initial quaternion

#include <BLE.h>


void setup() {
  initialize_pins();
  initialize_bluetooth();
  Serial.begin(115200);
  delay(3000);
  Wire.begin(SDA0_Pin, SCL0_Pin);

  // Initialize accelerometer
  int astatus;
  astatus = accel.begin();
  if (astatus < 0) {
    Serial.println("Accel Initialization Error");
    Serial.println(astatus);
    digitalWrite(ledPin, HIGH);
    delay(1000);
    digitalWrite(ledPin, LOW);
    delay(1000);
    digitalWrite(ledPin, HIGH);
    while (1) {}
  } else {
    Serial.println("Accelerometer initialized successfully");
  }

  // Initialize gyroscope
  astatus = gyro.begin();
  if (astatus < 0) {
    Serial.println("Gyro Initialization Error");
    Serial.println(astatus);
    digitalWrite(ledPin, HIGH);
    delay(1000);
    digitalWrite(ledPin, LOW);
    delay(1000);
    digitalWrite(ledPin, HIGH);
    while (1) {}
  } else {
    Serial.println("Gyroscope initialized successfully");
  }

  fusion.setup(accel.getAccelX_mss(), accel.getAccelY_mss(), accel.getAccelZ_mss());
  Serial.println();
  delay(100);
  gyro.readSensor();
  accel.readSensor();
  vec3_t gyro_rads(gyro.getGyroX_rads(), gyro.getGyroY_rads(), gyro.getGyroZ_rads());
  vec3_t curr_accel(accel.getAccelX_mss(), accel.getAccelY_mss(), accel.getAccelZ_mss());
  vec3_t filt_accel = ALPHA * curr_accel;
  fusion.update(gyro_rads.x, gyro_rads.y, gyro_rads.z, filt_accel.x, filt_accel.y, filt_accel.z);
  
  // Capture the initial quaternion
  initialQuat = fusion.getQuat();
  
  // Compute its conjugate
  initialQuatConjugate = {initialQuat.w, -initialQuat.v.x, -initialQuat.v.y, -initialQuat.v.z};
}

void loop() {
  gyro.readSensor();
  accel.readSensor();
  delay(10);
  vec3_t gyro_rads(gyro.getGyroX_rads(), gyro.getGyroY_rads(), gyro.getGyroZ_rads());
  vec3_t curr_accel(accel.getAccelX_mss(), accel.getAccelY_mss(), accel.getAccelZ_mss());
  vec3_t filt_accel = ALPHA * curr_accel;
  fusion.update(gyro_rads.x, gyro_rads.y, gyro_rads.z, filt_accel.x, filt_accel.y, filt_accel.z);
  
  // Get the current quaternion
  quat_t q = fusion.getQuat();
  
  // Correct the quaternion by multiplying with the initial conjugate
  q = initialQuatConjugate * q;
  
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
  Serial.println();
}