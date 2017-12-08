#include "commi2c.h"
#include "calibration.h"



const int ledPin = 9;
const int analogInPin = A0; // Analog input pin that the LDR is attached to

// Pin used to check if the arduino is the nº1 or nº2
const int idPin = 7;
// my i2c address
int myaddress = -1;

//temp?
int howLongToWait = 100;
int lastTimeItHappened = 0;
int howLongItsBeen = 0;

CommI2C* i2c = new CommI2C();
Calibration c1= Calibration(i2c, analogInPin, ledPin, -0.62, 1.96);

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
   Serial.print("tamanho e address = ");
   Serial.print(netSize);
   Serial.println(i2c->getAddr(0));

   //temp
   if(i2c->getAddrListSize() > 0) {
      c1.start_calibration();
   }
}

void loop() {
  if(i2c->recalibration == 1) {
    c1.cleanCalibVars();  //clean all variables used in calibration
    c1.start_calibration(); //starts a new calibration
  }
  
  
  
 /* howLongItsBeen = millis() - lastTimeItHappened;
  if(howLongItsBeen >= howLongToWait){

   
  }*/
}
