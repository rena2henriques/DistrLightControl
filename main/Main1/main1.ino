#include "pid.h"

const int analogInPin = A0; // Analog input pin that the potentiometer is attached to
const int analogOutPin = 9; // Analog output pin that the LED is attached to
int sensorValue = 0; // value read from the pot
int outputValue = 0.0; // value output to the PWM (analog out)
float lux = 0.0;
//actuatorMin, actuatorMax, ocupationlux, unocupationlux, ref, antiWgain, antiWFlag, deadFlag, deadMin, deadMax, FFWDFlag, kp, ki, kd, T
PID pid(-0.62, 1.96, 0, 255, 70, 35, 35, 0.74, 1, 1, -20, 20, 1, 1.35, 0.019, 0, 30);

// time variables (ms)
unsigned long currentTime = 0;
unsigned long previousTime = 0;
unsigned long sampleInterval = 30;

// string reading
char rx_byte = 0;
String rx_str = "";
char temp_str[20] = "";
char temp_fl[20] = "";

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
    }
      
    memset(temp_fl, 0, 20);
    memset(temp_str, 0, 20);  
}

void setup() {
  Serial.begin(115200);
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

  	sensorValue = analogRead(analogInPin);

  	lux = pid.vtolux(sensorValue);

  	outputValue = pid.calculate(lux);

  	analogWrite(analogOutPin, outputValue);

  	Serial.print(pid.getReference());
  	Serial.print(' ');
  	Serial.println(lux);
  	/*Serial.print(' ');
  	Serial.print( ( (float) outputValue/255)*100);
  	Serial.print(' ');
  	Serial.print(pid.getFFWDFlag());
  	Serial.print(' ');
  	Serial.println(currentTime);*/

  	previousTime = currentTime;
  }
}