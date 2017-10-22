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

// for transforming LUX to pwm
const float slope = 2.4356;
const float y_origin = -6.9365;

// serial inputs
float avg_constant = 0.5; 
float lux_ref = 70;

// serial auxiliars
String rx_str = "";
char temp_str[20] = "";
char temp_fl[20] = "";


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

// transforms a certain lux value into the correspondent pwm value
inline int getPwmValue(float lux_aux) {
	int amplitude = slope*lux_aux + y_origin;

  // pwm can't be more than 255 or less than 0
  if (amplitude > 255){
    amplitude = 255;
  } else if(amplitude <= 0) {
    amplitude = 0;
  }
  return amplitude;
}

void setup() {
  Serial.begin(9600); // initialize serial communications at 9600 bps
  analogWrite(analogOutPin, LOW);
}

void loop() {

	if (Serial.available() > 0) {    // is a character available?
      rx_str = Serial.readString();       // get the character

      char *rx_str_aux = rx_str.c_str();

      if(sscanf(rx_str_aux, "%[^ =] = %[^\n]", temp_str, temp_fl) != 2) {
         Serial.println(temp_str);
         Serial.println(temp_fl);
         Serial.println("Input Error: Wrong input syntax");
      }

      if ( strcmp(temp_str,"lux_ref") == 0){
        lux_ref = atof(temp_fl);
        // print the result
        Serial.print("The new value of lux_ref is ");
        Serial.println(lux_ref);
      } else if (strcmp(temp_str, "avg_constant") == 0) {
        avg_constant = atof(temp_fl);
        // print the result
        Serial.print("The new value of avg_constant is ");
        Serial.println(avg_constant);
      } else {
        Serial.println("Input Error: Variable not recognized");
      }

      memset(temp_fl, 0, 20);
      memset(temp_str, 0, 20);
      rx_str = "";                // clear the string for reuse
   
  } // end: if (Serial.available() > 0)

	sensorValue = analogRead(analogInPin); // read the analog in value
  avg_lux = average(avg_lux, vtolux(sensorValue));
		  
	// print lux values to serial
	Serial.print("lux = ");
	Serial.print(avg_lux);
  Serial.print("\t lux_Ref = ");
  Serial.print(lux_ref);
	Serial.print("\t sensor = ");
	Serial.print(sensorValue);
  Serial.print("\t time = ");
  Serial.print(millis());
  
	// FeedForward control function
	outputValue = getPwmValue(lux_ref);
	analogWrite(analogOutPin, outputValue); // change the analog out value

	Serial.print("\t PWM = ");
	Serial.println(outputValue);
	delay(50);// the professor recomended a sampling time of 30ms
}


