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
/*char temp_str[20] = "";
char temp_fl[20] = "";*/
char rpi_requestType;
char rpi_requestParam;
char rpi_nodeIndex;
int label_rpi;

//classes
CommI2C* i2c = new CommI2C();
Consensus c1= Consensus(i2c, analogInPin, ledPin, -0.62, 1.96, 1, 0, 60);
PID pid(-0.62, 1.96, 0, 255, 70, 35, 0.74, 1, 1, -0.7, 0.7, 1, 1.35, 0.019, 0, 30);

//just an empty string
char empty[] = "";

//evaluation metric variables
float energy =0.0;
float cError=0.0;
float vFlicker=0.0;
int Ncount=0;
float lux_penult=0.0;
float lux_antepenult=0.0;


// reads the serial buffer and changes the variables accordingly
void analyseString(String serial_string) {
    byte label_rpi;
    char *rx_str_aux = serial_string.c_str();
    rpi_requestType = rx_str_aux[0];
    if(rpi_requestType == 'r'){ //checks if the user requested a reset
         i2c->sendToAll((byte) 4, empty);   //tells other nodes to reset their calibration
         i2c->recalibration = 1;
         return;
    }
    rpi_requestParam = rx_str_aux[2];
    rpi_nodeIndex = rx_str_aux[4];

    if(rpi_requestType == 'g'){
      label_rpi = 5;
      Serial.println("label = 5");
    }
    else if(rpi_requestType == 's') {
      Serial.println("label = 6"); 
      label_rpi = 5;
    }
    else {
      Serial.println("Wrong input");} //pode se mandar isto para o rpi?
                                    //assumir que a flag b,c,d são feitas no rpi

    /*if(rpi_nodeIndex != myAddress)
      //send to rpi_nodeIndex, label x, data=rpi_requestParam
      else (im the one request)
        //pôr um flag a 1 que é a mesma que punha se fosse outro arduino qlq*/
    
    
    
    Serial.print("type = ");
    Serial.println(rpi_requestType);
    Serial.print("param = ");
    Serial.println(rpi_requestParam);
    Serial.print("index = ");
    Serial.println(rpi_nodeIndex);
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
   

   //temp
   if(i2c->getAddrListSize() > 0) {
      i2c->sendToAll((byte) 4, empty);   //tells other nodes to reset their calibration
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

  //if my addr == 1 ???
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
    pid.setFirstIterationON(); //to send the consensus signal control

  }

  //pid
  currentTime = millis();
  if(currentTime - previousTime > sampleInterval) {

    Ncount++;

    //keeping lux from the 2 previous iterations 
    if(Ncount>2)
      lux_antepenult=lux_penult;

    if(Ncount>1)
      lux_penult=lux;
    
    //computing energy comsumed - after first iteration
    if(Ncount>1)
    energy=energy+outputValue/255*(previousTime-currentTime);

    // LOW PASS filter
    for(i = 0; i < n_samples; i++)
        sensorValue = sensorValue + analogRead(analogInPin);

    avg_value = sensorValue/n_samples;

    // converts the voltage to Lux
    lux = pid.vtolux(avg_value);
       
    //comfort error - only when measured lux is below the reference lux
    if( pid.getReference()-lux >0){
      cError=cError+(pid.getReference()-lux);
    }

    //error flickering - only after the 3rd iteration
    if(Ncount>2)
    vFlicker=vFlicker+abs(lux-2*lux_penult+lux_antepenult)/(currentTime-previousTime)/(currentTime-previousTime);
    
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
