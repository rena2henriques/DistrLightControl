# include "pid.h"

const int analogInPin = A0; // Analog input pin that the LDR is attached to
const int analogOutPin = 9; // Analog output pin that the LED is attached to
int sensorValue = 0; // value read from the pot
int outputValue = 0.0; // value output to the PWM (analog out)
float lux = 0.0;


float slope = 3.3059;//
float y_origin = 2.1629;
//actuatorMin, actuatorMax, ocupationlux, unocupationlux, ref, antiWgain, antiWFlag, deadFlag, deadMin, deadMax, FFWDFlag, kp, ki, kd, T

//PID pid(-0.62, 1.96, 0, 255, 70, 35, 35, 0.74, 1, 1, -0.7, 0.7, 1, 1.35, 0.019, 0, 30);
PID pid(-0.74, 1.92, 0, 255, 70, 35, 35, 0.0, 0, 0, 0.0, 0.0, 1, 0, 0, 0, 30,slope,y_origin);


// time variables (ms)
unsigned long currentTime = 0;
unsigned long previousTime = 0;
unsigned long sampleInterval = 30;

// string reading
char rx_byte = 0;
String rx_str = "";
char temp_str[20] = "";
char temp_fl[20] = "";

int count=0;

// LOW PASS FILTER
float avg_value = 0.0;
int n_samples =30;
int i = 0;
int filter_flag=1;

// reads the serial buffer and changes the variables accordingly
void analyseString(String serial_string) {
    
    char *rx_str_aux = serial_string.c_str();
            
    sscanf(rx_str_aux, "%[^ =] = %[^\n]", temp_str, temp_fl);

      // new reference value
    if ( strcmp(temp_str,"lux_ref") == 0){
      pid.setReference(atof(temp_fl));
      // the desk it occupied
    } else if (strcmp(temp_str, "on") == 0) {
      pid.setReference(70);
      // desk is unoccupied
    } else if (strcmp(temp_str, "off") == 0) {
      pid.setReference(35);
      // anti-windup system is off
    } else if (strcmp(temp_str,"antiwindup_off") == 0) {
      pid.setAntiWindupMode(0);
      // anti-windup system is on
    } else if (strcmp(temp_str,"antiwindup_on") == 0) {
      pid.setAntiWindupMode(1);
      // feedforward is on
    } else if (strcmp(temp_str,"ffwd_on") == 0) {
      pid.setFFWDMode(1);
      // feedforward is off
    } else if (strcmp(temp_str,"ffwd_off") == 0) {
      pid.setFFWDMode(0);
      // deadzone is off
    } else if (strcmp(temp_str,"dead_off") == 0){
      pid.setDeadMode(0);
      // deadzone is on
    } else if (strcmp(temp_str,"dead_on") == 0) {
      pid.setDeadMode(1);
    } else if (strcmp(temp_str,"filter_on") == 0) {
      filter_flag=1;
      n_samples=30;
    } else if (strcmp(temp_str,"filter_off") == 0) {
      filter_flag=0;
      n_samples=1;
    }
    
      
    memset(temp_fl, 0, 20);
    memset(temp_str, 0, 20);  
}

void setup() {
  Serial.begin(115200);
  analogWrite(analogOutPin, 35);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {    // is a character available?
    rx_byte = Serial.read();       // get the character
    
    if (rx_byte != '\n') {
      // a character of the string was received
      rx_str += rx_byte;
    }
    // end of string
    else {
      // checks what serial buffer said
      analyseString(rx_str);
      rx_str = ""; // clear the string for reuse
    }
  }

  currentTime = millis();
  if(currentTime - previousTime > sampleInterval) {

    // LOW PASS filter
    for(i = 0; i < n_samples; i++)
        sensorValue = sensorValue + analogRead(analogInPin);

    avg_value = sensorValue/n_samples;

    // converts the voltage to Lux
  	lux = pid.vtolux(avg_value);

  	outputValue = pid.calculate(lux);

    // write the pwm to the LED
  	analogWrite(analogOutPin, outputValue);

  	Serial.print(pid.getReference());
  	Serial.print(' ');
  	Serial.print(lux);

  	Serial.print(' ');
  	Serial.print( ( (float) outputValue/255)*100);
  	Serial.print(' ');
  	Serial.print(pid.getFFWDFlag());
  	Serial.print(' ');
  	Serial.println(currentTime);

    // reset the read values
    sensorValue = 0;

    // reset the timer
    previousTime = currentTime;
  }
}
