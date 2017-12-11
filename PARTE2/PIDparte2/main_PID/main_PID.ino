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

//classes
CommI2C* i2c = new CommI2C();
Consensus c1= Consensus(i2c, analogInPin, ledPin, -0.62, 1.96, 1, 0, 35);
PID pid(-0.62, 1.96, 0, 255, 70, 35, 0.74, 1, 1, -0.7, 0.7, 1, 1.35, 0.019, 0, 30);

//just an empty string
char empty[] = "";


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
      pid.cleanvars();
      pid.setPwmConsensus(pwmconsensus); //pwm value for feedforward
      pid.setReference(c1.getRefConsensus()); //setting the new ref from Consensus

   }
}

void loop() {
  
  //recalibration
  if(i2c->recalibration == 1) {
    c1.cleanCalibVars();  //clean all variables used in calibration
    c1.start_calibration(); //starts a new calibration
    pwmconsensus = c1.consensusIter();   
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
    //lux=lux*1.425; <--- para novo ldr?

    outputValue = pid.calculate(lux);

    // write the pwm to the LED
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

    // reset the read values
    sensorValue = 0;

    // reset the timer
    previousTime = currentTime;
  }
}
