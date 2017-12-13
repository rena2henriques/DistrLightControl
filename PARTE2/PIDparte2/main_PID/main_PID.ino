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
char rpi_requestType[7];
char rpi_requestParam[7];
char rpi_nodeIndex[7];

//classes
CommI2C* i2c = new CommI2C();
Consensus c1= Consensus(i2c, analogInPin, ledPin, -0.62, 1.96, 1, 0, 35);
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

//other variables
int occupancyState=0;


//  get - analyses the string sent by arduino 1 and send to the raspb the value he requested 
void rpiAnalyser(rpi_requestParam){

  char label=rpi_requestParam[0];
  char[10] data;
  float requestedValue=0;
   switch(rpi_requestParam[0]) {
      case 'o':
          requestedValue=(float)occupancyState; //someone told me to read my ADC          
          break;
      case 'L':
          requestedVale=c1.getLowerRef();
          break;
      case 'r':
          requestedValue=c1.getRefConsensus(); //someone told me to read my ADC          
          break;
      case 'O':
          requestedVale=c1.getExternalIlluminance();
          break;     
      case 'e':
          requestedValue=energy; //someone told me to read my ADC          
          break;
      case 'c':
          requestedVale=cError;
          break;      
      case 'v':
          requestedVale=vFlicker;
          break;    
   }

  dtostrf(requestedValue, 10, 2,data);

  Serial.print("label:");
  Serial.println(label);  
  Serial.print("myaddress:");
  Serial.println(myaddress);
  Serial.print("data:");
  Serial.println(data);
  
  Wire.beginTransmission(rpiAddress);
  Wire.write(label);
  Wire.write(myaddress);
  Wire.write(data);
  Wire.endTransmission();
 
}

// reads the serial buffer and changes the variables accordingly
void analyseString(String serial_string) {
    byte label_rpi;
    byte dest;
    char *rx_str_aux = serial_string.c_str();
    sscanf(rx_str_aux, "%[^ ] %[^ ] %[^\n]", rpi_requestType, rpi_requestParam, rpi_nodeIndex); //stores the 3 paramets in chars[]
    dest = atoi(rpi_nodeIndex);  //array to int to byte
    if(dest != myaddress){ //send via i2c to node request
        if(rpi_requestType[0] == 'g'){
          label_rpi = 6;
          i2c->send(label_rpi, dest, rpi_requestParam);
        }else if (rpi_requestType[0] == 's') {
          label_rpi = 7;
          i2c->send(label_rpi, dest, rpi_requestParam);
        }else {
          Serial.println("Wrong input");
        } //pode se mandar isto para o rpi? assumir que a flag b,c,d são feitas no rpi
     }else{ //if rpi request my info, no i2c usage
      if(rpi_requestType[0] == 'g'){
        rpiAnalyser(rpi_requestParam); 
      }
      else if(rpi_requestType[0] == 's'){
        //set new reference
        }
     }  
   
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

  if(myaddress==1){
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
  }
  
  if(i2c->rpiFlagG == 1) {   //message received from rpi
    rpiAnalyser(i2c->rpiRequest); 
    i2c->rpiFlagG = 0;
    i2c->rpiRequest = "";
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
    energy=energy+(outputValue/255)*(previousTime-currentTime);

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
    vFlicker=vFlicker+(abs(lux-2*lux_penult+lux_antepenult))/(currentTime-previousTime)/(currentTime-previousTime);
    
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