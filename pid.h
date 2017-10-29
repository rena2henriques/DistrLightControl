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
	float c; // é o b que está nos slides, tem que ser entre 0 e 1
	float T;
	float a;
	float K1, K2, K3, K4;


	float iTerm, dTerm, pTerm; 
	float iTerm_prev, dTerm_prev; 
	float e_prev, lux_prev;
	float error;
	int u;

	int actuatorMax;
	int actuatorMin;
	// for transforming LUX to pwm
	const float slope = 2.4356;
	const float y_origin = -6.9365;

	// Voltage to lux
	const float a_lux = -0.74; // declive da reta aproximada do LDR
	const float b_lux = 1.92; // ordenada na origem da reta aproximada do LDR

	int sensorValue = 0; // VAI SER USADO?

	// anti-Windup Variables
	int errorWindup = 0;
	float gain_w = 1.35;
	int antiWindFlag;

	// constructor
	PID();
	PID(int actMin, int actMax, int ocupationMax, int ocupationMin, int ref, float antiWk, int antiFlag);
	
	int setSaturation(int output);
	
public:

	void setAntiWindupParam(float k);

	void setActuatorLimits(int min, int max);

	int getPwmValue(float lux);

	float vtolux(int sensorValue);

	// reference functions
	void setReference(int ref);
	int getReference();

	void setOcupationLux(int min, int max);

	void setPIDparameters(float kp, float ki, float kd);

	int calculate(float lux);
};


#endif