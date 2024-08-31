#ifndef PIN_DEFS_H
#define PIN_DEFS_H

#define SDA0_Pin 48
#define SCL0_Pin 47
#define led_pin 39
#define servo_x 1
#define servo_y 2
#define chip_select 5
#define select 6
#define buzzer 8
#define pyro_1 9
#define pyro_2 10
#define MOSI 11
#define MISO 13

inline void initializePins() {
  pinMode(pyro_1, OUTPUT);
  pinMode(pyro_2, OUTPUT);
  digitalWrite(pyro_1, LOW);
  digitalWrite(pyro_2, LOW);
  pinMode(select, INPUT);
  pinMode(led_pin, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(chip_select, OUTPUT);
  digitalWrite(chip_select, HIGH);
}

#endif