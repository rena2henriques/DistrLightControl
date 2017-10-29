const int analogInPin = A0; // Analog input pin that the potentiometer is attached to
const int analogOutPin = 9; // Analog output pin that the LED is attached to
int sensorValue = 0; // value read from the pot
int outputValue = 0.0; // value output to the PWM (analog out)
float r_ldr = 0.0;
float Vsensor=0.0;

// transform into double
float lux = 0.0;
float avg_lux = 0.0;
// for transforming V to lux
const float a_lux = -0.74; // declive da reta aproximada do LDR
const float b_lux = 1.92; // ordenada na origem da reta aproximada do LDR

// serial inputs
float avg_constant = 0.5; 
float lux_ref = 100;

// PI control variables
float erro=0.0;
int u=0;
//parameteres obtained 
//float Kp=1.35;
//float Ki=0.019;
float Kp=1.35;
float Ki=0.019; 
//float Ki=0.02846; //when T=20ms
float c=1.0; // é o b que está nos slides, tem que ser entre 0 e 1
float T= 30.0;
float K1=Kp*c;
float K2=Kp*Ki*T/2;

// time variables
unsigned long previousTime = 0; // ms
unsigned long sampleInterval = 30; // ms

float i=0.0, i_ant=0.0, e_ant=0.0;

inline float average(float avg, float new_value) {
  return avg_constant*avg + (1-avg_constant)*new_value;
}

// calculates the lux value from a certain sensorvalue
float vtolux(int sensorValue ){
  Vsensor = sensorValue*5.0/1024.0;
  r_ldr = 10.0*(5-Vsensor)/Vsensor;
  lux = pow(r_ldr/(pow(10,b_lux)), 1/a_lux);
  return lux; 
}

void setup() {
  Serial.begin(9600); // initialize serial communications at 9600 bps
}

void loop() {

  unsigned long currentTime = millis();
  if(currentTime - previousTime > sampleInterval) {
    sensorValue = analogRead(analogInPin); // read the analog in value
    // LOW pass filter 
    avg_lux = average(avg_lux, vtolux(sensorValue));

    Serial.println(avg_lux);
   // Serial.print('\t');
  
    erro=lux_ref-avg_lux;
    i=i_ant+K2*(erro+e_ant);
  
    //summing 0.5 to round
    u=(int) (K1*lux_ref-Kp*avg_lux+i+0.5);
    
    if(u>255)
      u=255;
    else if(u<0)
      u=0;

    //Serial.println(u);
    analogWrite(analogOutPin,u);
    //Serial.println(u);
    e_ant=erro;
    i_ant=i;

    previousTime = currentTime;
  }
}
