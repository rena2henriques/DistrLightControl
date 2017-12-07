#include "commi2c.h"
#include "consensus.h"
#include <Wire.h>
#include <LinkedList.h>

const int ledPin = 9;
const int analogInPin = A0; // Analog input pin that the LDR is attached to

// Pin used to check if the arduino is the nº1 or nº2
const int idPin = 7;
// my i2c address
int myaddress = -1;

CommI2C i2c(A0, 9); 

Consensus c1(1, 0.1, 100, -0.62,1.96);
//Consensus c1(1, 0.0, 80, -0.62,1.96);



// used only for the case of our problem (2 arduinos)
inline void idCheck(const int idPin) {
  // if pin idPin is HIGH = arduino nº1
  if(digitalRead(idPin) == HIGH)
    myaddress = 1; // I'm the arduino nº1
  else 
    myaddress = 2; // I'm not the arduino nº1
}
  

void receiveHandler(int numBytes) {

  // Initial ACK
  if (numBytes == 0)
    return;

  byte first8=0;
  byte last8=0;
  
  while(Wire.available() > 0) {
    // reads first received byte, shift right 8
    last8 = Wire.read();// << 8;
    first8 = Wire.read();  
  
    i2c.msgDecoder(last8, first8);
   }
}


void setup() {
  Serial.begin(115200);

  // gets i2c address from digital pin
  idCheck(idPin);

  i2c.setAddress(myaddress);

  Wire.begin(myaddress);
  
  // checks the number of nodes in the network and their address
  int nNodes = i2c.findNodes();
    // tells the system to recablibrate
  i2c.sendToAll((byte) 16, (byte) 0);
  
  // -----------------------------------
  Serial.print("n_nodes =");
  Serial.println(nNodes);

  // ---------------------------------

  delay(100);
  
  Wire.onReceive(receiveHandler);     

  // calibration of the network to get K values
   i2c.calibration();
   LinkedList<float> *ADCList = (i2c.getADCvalues()); //isto agora é com endereços 


   c1.setKmatrix(ADCList,(int)(100*200/255)); //setMatrix agora recebe ponteiro
 /* LinkedList<float> Klist = c1.getKlist();
  c1.setKmatrix_user(Klist);
  Klist.add(2);
  Klist.add(1);  
  
  c1.setO(0.0);*/
 int d= c1.consensusIter(myaddress,&i2c);

  Serial.print("pwm=");
  Serial.println(d);

  

}

void loop() {
  
  i2c.checkFlags();
  if(i2c.reconsensusFlag!=0){
    int d= c1.consensusIter(myaddress,&i2c);
    Serial.print("pwm1=");
    Serial.println(d); 
    i2c.reconsensusFlag=0;
  }
  
}
