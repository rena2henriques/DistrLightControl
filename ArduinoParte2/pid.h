#ifndef PID_CONTROL
#define PID_CONTROL

// meter termo derivativo mas Kd a 0 para o nosso caso
// só para ser uma classe mais geral

class PID {
private:
	float reference;

	int occupiedLux;
	int unnocupiedLux;

	//parameteres obtained 
	float Kp; // proporcional gain
	float Ki; // integral gain
	float Kd; // derivative gain
	float c = 1.0; // é o b que está nos slides, tem que ser entre 0 e 1
	float T;
	float a = 1.0;
	float K1, K2, K3, K4;


	float iTerm, dTerm, pTerm; 
	float iTerm_prev, dTerm_prev; 
	float e_prev, lux_prev;
	float error;
	int u;
  int uConsensus;

	int actuatorMax;
	int actuatorMin;
	// for transforming LUX to pwm
 

	// Voltage to lux
	float a_lux = 0.0; // declive da reta aproximada do LDR
	float b_lux = 0.0; // ordenada na origem da reta aproximada do LDR

	int sensorValue = 0; // VAI SER USADO?

	// anti-Windup Variables
	int errorWindup = 0;
	float gain_w;
	int antiWindFlag;

	//FFWD Variables
  	int FFWDFlag; // checks if ffwd is being used
  	int first_iteration; // first iteration of a certain ref

	// deadzone variables
	int deadFlag; // 0 if deadzone off, 1 if on
	float dead_max;
	float dead_min;

	void deadzone();

	int setSaturation(int output);
	
public:

	// constructor
	PID();
	PID(float a_ldr, float b_ldr, int actMin, int actMax, int ocupationMax, int ocupationMin, float antiWk, 
														int antiFlag, int deadflag, float deadMin, float deadMax, int FFWD_flag, float kp, float ki, float kd, float samplingTime);
	void setAntiWindupParam(float k);
	void setActuatorLimits(int min, int max);
	void setAntiWindupMode(int mode);
	void setFFWDMode(int FFWDmode);
  	int getFFWDFlag();
  	void setDeadMode(int deadmode);

	void setPwmConsensus(float dconsensus);
	float vtolux(int sensorValue);

	void setSamplingTime(float samplingTime);

	// reference functions
	void setReference(float ref);
	int getReference();

	void setOcupationLux(int min, int max);

	void setPIDparameters(float kp, float ki, float kd);

  void setFirstIterationON();

	int calculate(float lux);
 
  void cleanvars();

};



#endif
