#include <gpio.h>
#include <softPwm.h>
#include <wiringPi.h>

#define RESISTOR 4
#define FAN 5

void turnOnResistor(int resistorValue) {
  pinMode(RESISTOR, OUTPUT);
  softPwmCreate(RESISTOR, 0, 100);
  softPwmWrite(RESISTOR, resistorValue);
}

void turnOffResistor() {
  pinMode(RESISTOR, OUTPUT);
  softPwmCreate(RESISTOR, 0, 100);
  softPwmWrite(RESISTOR, 0);
}

void turnOnFan(int fanValue) {
  pinMode(FAN, OUTPUT);
  softPwmCreate(FAN, 0, 100);
  softPwmWrite(FAN, fanValue);
}

void turnOffFan() {
  pinMode(FAN, OUTPUT);
  softPwmCreate(FAN, 0, 100);
  softPwmWrite(FAN, 0);
}

void pwmControl(int intensity_signal) {
  if (intensity_signal > 0) {
    turnResistanceOn(intensity_signal);
    turnFanOff();
  } else {
    if (intensity_signal <= -40)
      turnFanOn(intensity_signal * -1);
    else
      turnFanOff();
    turnResistanceOff();
  }
}