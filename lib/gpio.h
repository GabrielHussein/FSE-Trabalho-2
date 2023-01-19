#ifndef GPIO_H_
#define GPIO_H_

void turnOffResistor();
void turnOnResistor(int resistorValue);
void turnOffFan();
void turnOnFan(int fanValue);
void pwmControl(int intensity_signal);

#endif /* GPIO_H_ */