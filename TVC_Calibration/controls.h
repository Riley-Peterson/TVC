#ifndef CONTROLS_H
#define CONTROLS_H

#include <PID_v1.h>
#include <ESP32Servo.h>

// Servo objects
Servo servoX;
Servo servoY;

// PID variables
double firstPitchSetpoint = 0, pitchInput, pitchOutput;
double firstYawSetpoint = 0, yawInput, yawOutput;
double secondPitchSetpoint = 30;
double secondYawSetpoint = 30;

double Kp = 1, Ki = 0.00, Kd = 0.3; //Ki originally 0.02

float actuation_x = 0;
float actuation_y = 0;

// PID controllers
PID firstPitchPID(&pitchInput, &pitchOutput, &firstPitchSetpoint, Kp, Ki, Kd, DIRECT);
PID firstYawPID(&yawInput, &yawOutput, &firstYawSetpoint, Kp, Ki, Kd, DIRECT);
PID secondPitchPID(&pitchInput, &pitchOutput, &secondPitchSetpoint, Kp, Ki, Kd, DIRECT);
PID secondYawPID(&pitchInput, &pitchOutput, &secondYawSetpoint, Kp, Ki, Kd, DIRECT);

inline void initializeControls() {
  // Attach servos
  servoX.attach(1); // GPIO 1 for pitch (x-axis)
  servoY.attach(2); // GPIO 2 for yaw (y-axis)

  // Initialize PID controllers
  firstPitchPID.SetMode(AUTOMATIC);
  firstPitchPID.SetOutputLimits(-25, 25); // Limits for servo angles
  firstYawPID.SetMode(AUTOMATIC);
  firstYawPID.SetOutputLimits(-25, 25); // Limits for servo angles
  secondPitchPID.SetMode(AUTOMATIC);
  secondPitchPID.SetOutputLimits(-25, 25); // Limits for servo angles
  secondYawPID.SetMode(AUTOMATIC);
  secondYawPID.SetOutputLimits(-25, 25); // Limits for servo angles
}

inline void updateFirstControls() {
  // Update PID inputs
  pitchInput = pitch;
  yawInput = yaw;

  // Compute PID outputs
  firstPitchPID.Compute();
  firstYawPID.Compute();

  // Move servos based on PID output
  actuation_x = pitchOutput;
  actuation_y = yawOutput;
  servoX.write(map(pitchOutput, -25, 25, 65, 115));
  servoY.write(map(yawOutput, -25, 25, 65, 115));
}

inline void updateSecondControls() {
  // Update PID inputs
  pitchInput = pitch;
  yawInput = yaw;

  // Compute PID outputs
  secondPitchPID.Compute();
  secondYawPID.Compute();

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