#ifndef CONTROLS_H
#define CONTROLS_H

#include <PID_v1.h>
#include <ESP32Servo.h>

// Servo objects
Servo servoX;
Servo servoY;

// PID variables
double pitchSetpoint = 0, pitchInput, pitchOutput;
double yawSetpoint = 0, yawInput, yawOutput;
double Kp = 0.25, Ki = 0.0, Kd = 0.07;

float actuation_x = 0;
float actuation_y = 0;

// PID controllers
PID pitchPID(&pitchInput, &pitchOutput, &pitchSetpoint, Kp, Ki, Kd, DIRECT);
PID yawPID(&yawInput, &yawOutput, &yawSetpoint, Kp, Ki, Kd, DIRECT);

inline void initializeControls() {
  // Attach servos
  servoX.attach(1); // GPIO 1 for pitch (x-axis)
  servoY.attach(2); // GPIO 2 for yaw (y-axis)

  // Initialize PID controllers
  pitchPID.SetMode(AUTOMATIC);
  pitchPID.SetOutputLimits(-25, 25); // Limits for servo angles
  yawPID.SetMode(AUTOMATIC);
  yawPID.SetOutputLimits(-25, 25); // Limits for servo angles
}

inline void updateControls() {
  // Update PID inputs
  pitchInput = pitch;
  yawInput = yaw;

  // Compute PID outputs
  pitchPID.Compute();
  yawPID.Compute();

  // Move servos based on PID output
  actuation_x = pitchOutput;
  actuation_y = yawOutput;
  servoX.write(map(pitchOutput, -25, 25, 65, 115));
  servoY.write(map(yawOutput, -25, 25, 65, 115));
}

bool launchDetect() {
  if(getAccelMagnitude() > 25.0) {
    Serial.println("Launch Detected!");
    return true;
  }
  return false;
}

#endif