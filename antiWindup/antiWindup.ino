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
float lux_ref = 70;

// PI control variables
float erro=0.0;
int u=0;
//parameteres obtained 
float Kp=1.35;
float Ki=0.019;
float c=1.0; // é o b que está nos slides, tem que ser entre 0 e 1
float T= 30.0;
float K1=Kp*c;
float K2=Kp*Ki*T/2;
float iTerm=0.0, iTerm_ant=0.0, e_ant=0.0;

// anti-Windup Variables
const int outMax = 255;
const int outMin = 0;
int erroWindup = 0;
float gain_w = 1.35;

// time variables (ms)
unsigned long currentTime = 0;
unsigned long previousTime = 0;
unsigned long sampleInterval = 30;

// serial auxiliars
String rx_str = "";
char temp_str[20] = "";
char temp_fl[20] = "";

inline float average(float avg, float new_value) {
  return avg_constant*avg + (1-avg_constant)*new_value;
}

int setSaturation(int output) {
    if(output > outMax){
      erroWindup = outMax - output;
      output = outMax;
    } else if(output < outMin){
      erroWindup = outMin - output;
      output = outMin;
    } else {
       erroWindup = 0;
    }
    return output;
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

    if (Serial.available() > 0) {    // is a character available?
      rx_str = Serial.readString();       // get the character

      char *rx_str_aux = rx_str.c_str();

      sscanf(rx_str_aux, "%[^ =] = %[^\n]", temp_str, temp_fl);

      if ( strcmp(temp_str,"lux_ref") == 0){
        lux_ref = atof(temp_fl);
        // print the result
      } else if (strcmp(temp_str, "avg_constant") == 0) {
        avg_constant = atof(temp_fl);
      // desk is occupied
      } else if (strcmp(temp_str, "on") == 0) {
        lux_ref = 70;
      // desk is unoccupied
      } else if (strcmp(temp_str, "off") == 0) {
        lux_ref = 30;
      }

      memset(temp_fl, 0, 20);
      memset(temp_str, 0, 20);
      rx_str = "";                // clear the string for reuse
   
  } // end: if (Serial.available() > 0)
  
  currentTime = millis();
  if(currentTime - previousTime > sampleInterval) {
    sensorValue = analogRead(analogInPin); // read the analog in value
    // LOW pass filter 
    avg_lux = average(avg_lux, vtolux(sensorValue));

    Serial.print(avg_lux);
    Serial.print('\t');
    
    erro=lux_ref-avg_lux;

    iTerm=iTerm_ant+K2*(erro+e_ant) + gain_w*erroWindup;

    Serial.println(iTerm);

    //summing 0.5 to round
    u = (int) (K1*lux_ref-Kp*avg_lux+iTerm+0.5);
    
    // Limit the Output because of the saturation of
    // the actuator
    u = setSaturation(u);

    /*if(u > outMax){
      u = outMax;
    } else if(u < outMin){
      u = outMin;
    }*/
    
    //Serial.println(u);
    
    analogWrite(analogOutPin,u);
    
    e_ant=erro;
    iTerm_ant=iTerm;

    previousTime = currentTime;
  }
}
