#ifndef PIN_DEFS_H
#define PIN_DEFS_H

#define SDA0_Pin 48
#define SCL0_Pin 47
#define ledPin 39
#define ServoX 1
#define ServoY 2
#define CS 5
#define select 6
#define buzz 8
#define pyro1 9
#define pyro2 10
#define MOSI 11
#define MISO 13

inline void initialize_pins() {
  pinMode(pyro1, OUTPUT);
  pinMode(pyro2, OUTPUT);
  digitalWrite(pyro1, LOW);
  digitalWrite(pyro2, LOW);
  pinMode(ledPin, OUTPUT);
  pinMode(buzz, OUTPUT);
  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);
}

#endif