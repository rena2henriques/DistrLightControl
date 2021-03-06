#include "pid.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
using namespace std;

PID::PID() {

  // Parameters of the LDR
  a_lux = -0.74;
  b_lux = 1.92;

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
  	uConsensus=0;

  deadFlag = 1;
  dead_max = 1;
  dead_min = -1;


	antiWindFlag = 1;
  	FFWDFlag=1;

	setSamplingTime(30);

	setOcupationLux(35, 70);

	setAntiWindupParam(0.74);

	setPIDparameters(1.35, 0.019, 0);
}

PID::PID(float a_ldr, float b_ldr, int actMin, int actMax, int ocupationMax, int ocupationMin,
                             float antiWk,int antiFlag, int deadflag, float deadMin, float deadMax, int FFW_flag, float kp, float ki, float kd, float samplingTime) {
	
  // Parameters of the LDR
  a_lux = a_ldr;
  b_lux = b_ldr;

  // actuator saturation limits
	actuatorMin = actMin;
	actuatorMax = actMax;

  lux_prev = 0;

  iTerm = 0; 
  dTerm = 0;
  pTerm = 0; 
  iTerm_prev = 0;
  dTerm_prev = 0;
  e_prev = 0;
  error = 0;
  u = 0;
  uConsensus=0;

  deadFlag = deadflag;

  dead_min = deadMin;
  dead_max = deadMax;

	antiWindFlag = antiFlag;
  FFWDFlag=FFW_flag;

  setSamplingTime(samplingTime);

	setOcupationLux(ocupationMin, ocupationMax);

	setAntiWindupParam(antiWk);

  setPIDparameters(kp, ki, kd);

}

void PID::setPwmConsensus(float dconsensus){
  uConsensus=dconsensus*(255/100);
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

	// for using the feedforward control
  	first_iteration=1;
}

int PID::getReference() {
	return (reference+0.5); //arredondamento
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
	if(mode == 1){
		antiWindFlag = 1;
	} else if (mode==0){
		antiWindFlag = 0;
	}
}

// defines if we are using PID with feedforward or not
void PID::setFFWDMode(int FFWDmode){
  if(FFWDmode==1){
    FFWDFlag = 1;
  } else if (FFWDmode==0){
    FFWDFlag = 0;
    uConsensus=0;
  }
}

void PID::setDeadMode(int deadmode) {
  deadFlag = deadmode;
}

void PID::setFirstIterationON(){
  first_iteration=1;
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

// used to check if feedforward is being used or not
int PID::getFFWDFlag(){
  return FFWDFlag;
}

// deadzone used to lessen the error
void PID::deadzone() {
  if (error <= dead_max && error >= dead_min){
    error = 0;
  } else if (error > dead_max) {
    error = error - dead_max;
  } else if (error < dead_min) {
    error = error - dead_min;
  }
}

// the PID control system per se
int PID::calculate(float lux) {

  if(first_iteration == 1  && FFWDFlag==1) {//only if it's first iteration and FFWD is activated
        u = uConsensus;                     //so recalcular FFWD_output se se mudar lux_ref pelo serial
        first_iteration = 0;
	} else {   
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
	  u = (int) (pTerm + iTerm + dTerm + uConsensus + 0.5); // summing 0.5 to round

	  // windup system
	  u = setSaturation(u);
  }

  // continuity
	lux_prev = lux;
	iTerm_prev = iTerm;
	dTerm_prev = dTerm;
	e_prev = error;

	return u;
}

void PID::cleanvars(){

  iTerm=0;
  dTerm=0; 
  pTerm=0; 
  iTerm_prev=0; 
  dTerm_prev=0; 
  e_prev=0;
  lux_prev=0;
  error=0;
  u=0;
  errorWindup=0;

}

