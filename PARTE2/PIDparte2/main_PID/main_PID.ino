#include "commi2c.h"
#include "consensus.h"
# include "pid.h"


const int ledPin = 9;
const int analogInPin = A0; // Analog input pin that the LDR is attached to


// -----PID variables
int sensorValue = 0; // value read from the pot
int outputValue = 0.0; // value output to the PWM (analog out)
float lux = 0.0;
// time variables (ms)
unsigned long currentTime = 0;
unsigned long previousTime = 0;
unsigned long sampleInterval = 30;
// low pass filter variables
float avg_value = 0.0;
int n_samples =30;
int i = 0;
int filter_flag=1;

// Pin used to check if the arduino is the nº1 or nº2
const int idPin = 7;
// my i2c address
int myaddress = -1;

//temp?
int howLongToWait = 100;
int lastTimeItHappened = 0;
int howLongItsBeen = 0;

// string reading
char rx_byte = 0;
String rx_str = "";
char temp_str[20] = "";
char temp_fl[20] = "";

//classes
CommI2C* i2c = new CommI2C();
Consensus c1= Consensus(i2c, analogInPin, ledPin, -0.62, 1.96, 1, 0, 100);
PID pid(-0.62, 1.96, 0, 255, 70, 35, 0.74, 1, 1, -0.7, 0.7, 1, 1.35, 0.019, 0, 30);

//just an empty string
char empty[] = "";


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

void receiveHandler(int howMany) {
  int label;
  int src_addr;
  char c;
  String data;
  
  if(Wire.available())  {  
      label = Wire.read();    //first byte is a label   
      src_addr = Wire.read(); //second byte is the address of the sender
   }
   while (Wire.available()) { //remaining byte are data values
     c = Wire.read();
     data += c;       
   }

   i2c->msgDecoder(label, src_addr, data);
  
}

//consensus variables
int pwmconsensus = 0;


// used only for the case of our problem (2 arduinos)
inline void idCheck(const int idPin) {
  // if pin idPin is HIGH = arduino nº1
  if(digitalRead(idPin) == HIGH)
    myaddress = 1; // I'm the arduino nº1
  else 
    myaddress = 2; // I'm not the arduino nº2
}

void setup() {
   Serial.begin(115200);
   // gets i2c address from digital pin
   idCheck(idPin);
   i2c->setMyAddress(myaddress);
   c1.setMyAddress(myaddress);
    
   Wire.begin(myaddress);
   Wire.onReceive(receiveHandler);

   int netSize = i2c->findNodes();    //finds all nodes in the network
   i2c->sendToAll((byte) 4, empty);   //tells other nodes to reset their calibration

   //temp
   if(i2c->getAddrListSize() > 0) {
      c1.start_calibration();
      pwmconsensus = c1.consensusIter();
      Serial.print("Pwm=");
      Serial.println(pwmconsensus);
      Serial.print("luxconsensus=");
      Serial.println(c1.getRefConsensus());
      pid.cleanvars();
      pid.setPwmConsensus(pwmconsensus); //pwm value for feedforward
      pid.setReference(c1.getRefConsensus()); //setting the new ref from Consensus

   }
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

  
  //recalibration
  if(i2c->recalibration == 1) {
    c1.cleanCalibVars();  //clean all variables used in calibration
    c1.start_calibration(); //starts a new calibration
    pwmconsensus = c1.consensusIter();   
    Serial.print("Pwm=");
    Serial.println(pwmconsensus);
    Serial.print("luxconsensus=");
    Serial.println(c1.getRefConsensus());
    pid.cleanvars();
    pid.setPwmConsensus(pwmconsensus); //pwm value for feedforward
    pid.setReference(c1.getRefConsensus()); //setting the new ref from Consensus
  }

  //pid
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
    analogWrite(ledPin, outputValue);

    Serial.print(pid.getReference());
    Serial.print(' ');
    Serial.println(lux);

    /*Serial.print(' ');
    Serial.print( ( (float) outputValue/255)*100);    
    Serial.print(' ');
    Serial.print(pid.getFFWDFlag());
    Serial.print(' ');
    Serial.println(currentTime);*/

    // reset the read values
    sensorValue = 0;

    // reset the timer
    previousTime = currentTime;
  }
}
