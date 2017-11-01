#include "pid.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
using namespace std;

PID::PID() {
	// actuator saturation limits
	actuatorMin = 0;
	actuatorMax = 255;

  lux_prev = 0;

  iTerm = 0; 
  dTerm = 0;
  pTerm = 0; 
  iTerm_prev = 0;
  dTerm_prev = 0;
  e_prev = 0;
  error = 0;
  u = 0;

  deadFlag = 1;
  dead_max = 2;
  dead_min = -2;

	antiWindFlag = 1;

  setSamplingTime(30);

	setOcupationLux(35, 70);

	setReference(35);

	setAntiWindupParam(0.74);

  setPIDparameters(1.35, 0.019, 0);

}

PID::PID(int actMin, int actMax, int ocupationMax, int ocupationMin, int ref,
                             float antiWk,int antiFlag, int deadflag, float deadMin, float deadMax, float kp, float ki, float kd, float samplingTime) {
	// actuator saturation limits
	actuatorMin = actMin;
	actuatorMax = actMax;

  deadFlag = deadflag;

  dead_min = deadMin;
  dead_max = deadMax;

  lux_prev = 0;

  iTerm = 0; 
  dTerm = 0;
  pTerm = 0; 
  iTerm_prev = 0;
  dTerm_prev = 0;
  e_prev = 0;
  error = 0;
  u = 0;

	antiWindFlag = antiFlag;

  setSamplingTime(samplingTime);

	setOcupationLux(ocupationMin, ocupationMax);

	setReference(ref);

	setAntiWindupParam(antiWk);

  setPIDparameters(kp, ki, kd);

}

// Transforms a certain lux value to pwm using a function fit -> feedforward
int PID::getPwmValue(float lux) {

  int amplitude = slope*lux + y_origin;

  // pwm can't be more than 255 or less than 0
  if (amplitude > actuatorMax){
    amplitude = actuatorMax;
  } else if(amplitude <= actuatorMin) { 
    amplitude = actuatorMin;
  }
  return amplitude;
}

void PID::setSamplingTime(float samplingTime){
  T = samplingTime;
}

// sets the limits that the actuator starts saturating
void PID::setActuatorLimits(int min, int max){
	actuatorMin = min;
	actuatorMax = max;
}

// AntiWindup system
int PID::setSaturation(int output) {
    if(output > actuatorMax){
      if(antiWindFlag == 1){
      	errorWindup = actuatorMax - output;
      }
      output = actuatorMax;
    } else if(output < actuatorMin){
      if(antiWindFlag == 1){
      	errorWindup = actuatorMin - output;
      }
      output = actuatorMin;
    } else {
       errorWindup = 0;
    }
    return output;
}

// calculates the lux value from a certain sensorvalue
float PID::vtolux(int sensorValue){
  // bits to voltage
  float Vsensor = sensorValue*5.0/1024.0;
  // voltage to resistance
  float r_ldr = 10.0*(5-Vsensor)/Vsensor;
  // resistance to lux
  return pow(r_ldr/(pow(10,b_lux)), 1/a_lux); 
}

void PID::setReference(float ref) {
	reference = ref;
}

int PID::getReference() {
	return reference;
}

void PID::setOcupationLux(int min, int max) {
	unnocupiedLux = min;
	occupiedLux = max;
}

void PID::setAntiWindupParam(float k) {
	gain_w = k;
}

// sets the anti windup system on and off (1=on, 0=off)
void PID::setAntiWindupMode(int mode) {
	if(antiWindFlag == 1){
		antiWindFlag = 1;
	} else if (antiWindFlag){
		antiWindFlag = 0;
	}
}

void PID::setPIDparameters(float kp, float ki, float kd) {
	// control parameters
	Kp = kp;
	Ki = ki;
	Kd = kd;

	K1 = Kp * c;
	K2 = Kp * Ki * T/2;
	K3 = Kd/(Kd+a*T);
	K4 = Kp*Kd*a/(Kd+a*T);
}

void deadzone() {

  if (error >= dead_min && error <= dead_max){
    error = 0;
  } 
}

int PID::calculate(float lux) {

	// calculation of the error between the output and the objective
	error = reference - lux;

  if (deadFlag == 1){
    deadzone();
  }

	// calculation of the proportional term of PID
	pTerm = K1*reference-Kp*lux;

  // calculation of the integral term of PID
  iTerm= iTerm_prev + K2*(error + e_prev) + gain_w*errorWindup;

  // calculation of the derivative term of PID
  dTerm = K3*dTerm_prev - K4*(lux - lux_prev);

  // calculation of the Output of PID
  u = (int) (pTerm + iTerm + dTerm + 0.5); // summing 0.5 to round

  // windup system
  u = setSaturation(u);

  // continuity
  lux_prev = lux;
  iTerm_prev = iTerm;
  dTerm_prev = dTerm;
  e_prev = error;

  return u;
}
