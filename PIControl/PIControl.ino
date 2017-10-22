const int analogInPin = A0; // Analog input pin that the potentiometer is attached to
const int analogOutPin = 9; // Analog output pin that the LED is attached to
int sensorValue = 0;        // value read from the ldr

float rLdr = 0.0;
float vSensor=0.0;
// transform into double
float lux = 0.0;
const float a = -0.74; // declive da reta aproximada do LDR
const float b = 1.92; // ordenada na origem da reta aproximada do LDR

float erro=0.0;
int ref=70;
int u=0;

//parameteres obtained 
float Kp=1.35;
float Ki=0.019;

float c=1.0; // é o b que está nos slides, tem que ser entre 0 e 1
float T=  .0;
float K1=Kp*c;
float K2=Kp*Ki*T/2;

float i=0.0, i_ant=0.0, e_ant=0.0;

void setup() {
  Serial.begin(9600); // initialize serial communications at 9600 bps
}

void loop() {

  sensorValue = analogRead(analogInPin);
  
  vSensor = sensorValue*5.0/1024.0;
  rLdr = 10.0*(5-vSensor)/vSensor;
  lux = pow(rLdr/(pow(10,b)), 1/a); 
  Serial.println(lux);

  erro=ref-lux;
  i=i_ant+K2*(erro+e_ant);

  //summing 0.5 to round
  u=(int) (K1*ref-Kp*lux+i+0.5);
  
  if(u>255)
    u=255;
  else if(u<0)
    u=0;
    
  analogWrite(analogOutPin,u);
  
  e_ant=erro;
  i_ant=i;
  
  delay(30);
  
 
}
