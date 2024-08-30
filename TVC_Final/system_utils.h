#ifndef SYSTEM_UTILS_H
#define SYSTEM_UTILS_H

inline void blinkSlow(int numTimes) {
  for(int i = 0; i < numTimes; i++) {
    digitalWrite(led_pin, HIGH);
    delay(1000);
    digitalWrite(led_pin, LOW);
    delay(1000);
  }
}

inline void blinkFast(int numTimes) {
  for(int i = 0; i < numTimes; i++) {
    digitalWrite(led_pin, HIGH);
    delay(250);
    digitalWrite(led_pin, LOW);
    delay(250);
  }
}

inline void deviceConnectedSound() {
  tone(buzzer, 500);
  delay(250);
  noTone(buzzer);
  delay(100);
  tone(buzzer, 2000);
  delay(500);
  noTone(buzzer);
}

inline void deviceDisconnectedSound() {
  tone(buzzer, 2000);
  delay(250);
  noTone(buzzer);
  delay(100);
  tone(buzzer, 500);
  delay(500);
  noTone(buzzer);
}

inline void armedSound() {
  tone(buzzer, 2000);
  delay(100);
  noTone(buzzer);
  delay(150);
  tone(buzzer, 2000);
  delay(1000);
  noTone(buzzer);
}

inline void onGround() {
  while(1) {
    tone(buzzer, 1000);
    digitalWrite(led_pin, HIGH);
    delay(100);
    noTone(buzzer);
    digitalWrite(led_pin, LOW);
    delay(100);
    tone(buzzer, 1000);
    digitalWrite(led_pin, HIGH);
    delay(100);
    noTone(buzzer);
    digitalWrite(led_pin, LOW);
    delay(1000);
  }
}

#endif