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
float c=1.0; // � o b que est� nos slides, tem que ser entre 0 e 1
float T= 30.0;
float K1=Kp*c;
float K2=Kp*Ki*T/2;
float iTerm=0.0, iTerm_ant=0.0, e_ant=0.0;

// anti-Windup Variables
const int outMax = 255;
const int outMin = 0;
int erroWindup = 0;
float gain_w = 0.7;
int antiWindup_flag = 0; 

// time variables (ms)
unsigned long currentTime = 0;
unsigned long previousTime = 0;
unsigned long sampleInterval = 30;

// serial auxiliars
char rx_byte = 0;
String rx_str = "";
char temp_str[20] = "";
char temp_fl[20] = "";

// for transforming LUX to pwm, FFWD
const float slope = 2.4356;
const float y_origin = -6.9365;

//FFWD + PID variables
int first_iteration = 1;
int uFFWD = 0;
int uPID = 0;
int counter = 0;

// Low Pass filter
inline float average(float avg, float new_value) {
  return avg_constant*avg + (1-avg_constant)*new_value;
}


//fit from FFWD
inline int getPwmValue(float lux_aux) {
  int amplitude = slope*lux_aux + y_origin;
  return amplitude;
}

// AntiWindup system
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

// reads the serial buffer and changes the variables accordingly
void analyseString(String serial_string) {
    
    char *rx_str_aux = serial_string.c_str();
            
    sscanf(rx_str_aux, "%[^ =] = %[^\n]", temp_str, temp_fl);

    if ( strcmp(temp_str,"lux_ref") == 0){
      lux_ref = atof(temp_fl);
      first_iteration = 1; 
      // print the result
    } else if (strcmp(temp_str, "avg_constant") == 0) {
      avg_constant = atof(temp_fl);
      // desk is occupied
    } else if (strcmp(temp_str, "on") == 0) {
      lux_ref = 70;
      // desk is unoccupied
    } else if (strcmp(temp_str, "off") == 0) {
      lux_ref = 30;
      // anti-windup system is off
    } else if (strcmp(temp_str,"antiwindup_off") == 0) {
      antiWindup_flag = 0;
      // anti-windup system is on
    } else if (strcmp(temp_str,"antiwindup_on") == 0) {
      antiWindup_flag = 1;
    }
      
    memset(temp_fl, 0, 20);
    memset(temp_str, 0, 20);  
}

// calculates the lux value from a certain sensorvalue
float vtolux(int sensorValue ){
  Vsensor = sensorValue*5.0/1024.0;
  r_ldr = 10.0*(5-Vsensor)/Vsensor;
  lux = pow(r_ldr/(pow(10,b_lux)), 1/a_lux);
  return lux; 
}

void setup() {
  Serial.begin(115200); // initialize serial communications at 115200 bps
  analogWrite(analogOutPin,getPwmValue(lux_ref));
}

void loop() {
  
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
    sensorValue = analogRead(analogInPin); // read the analog in value
    // LOW pass filter 
    avg_lux = average(avg_lux, vtolux(sensorValue));

    Serial.print(avg_lux);
    Serial.print('\t');
    Serial.println(lux_ref);
   

    if(first_iteration == 1) {//first iteration, only FFWD, ignore PID
        uFFWD = getPwmValue(lux_ref); //output to be sent
        u = uFFWD;                     //so recalcular FFWD_output se se mudar lux_ref pelo serial
        first_iteration = 0;
        sampleInterval = 30;
    } else {    
        // calculation of error between ref and the present lux
        erro=lux_ref-avg_lux;
       // if ((int)erro != 0) { //case yes, use PID, case not, ?keep the same u value? faz sentido ou este if é desnecessario??, testar esta condição no antiwindup sem FFWD ver se muda alg coisa
            // calculation of the integral term of PI
            iTerm=iTerm_ant+K2*(erro+e_ant) + gain_w*erroWindup;

             // calculation of the Output of PI
             // summing 0.5 to round
             uPID = (int) (K1*lux_ref-Kp*avg_lux+iTerm+0.5);
             u = uPID + uFFWD;
             // AntiWindup System because of the saturation of the actuator
             if ( antiWindup_flag == 1 ){
               u = setSaturation(u);
             }
      //  }
      }
      e_ant=erro;
      iTerm_ant=iTerm;
      analogWrite(analogOutPin,u);
      counter++;
     /* if(counter == 100 || counter == 400) {
        sampleInterval = 200;
        lux_ref = 35;
        first_iteration = 1;
      }
      if(counter == 250 || counter == 650) {
        sampleInterval = 200;
        lux_ref = 70;
        first_iteration = 1;
      }*/
      // saves variables for next loop
      
      previousTime = currentTime;
     
   }
}
