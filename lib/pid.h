#ifndef PID_H_
#define PID_H_

void pidSetupConstants(double Kp_, double Ki_, double Kd_);
void pidUpdateReferences(float reference_);
double pidControl(double outMeasurement);

#endif /* PID_H_ */
