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

CommI2C* i2c = new CommI2C(analogInPin, ledPin);


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
    
   Wire.begin(myaddress);
   Wire.onReceive(receiveHandler);

   int netSize = i2c->findNodes();
   Serial.print("tamanho e address = ");
   Serial.print(netSize);
   Serial.println(i2c->getAddr(0));

   //temp
   Calibration c1= Calibration(i2c, myaddress);
}

void loop() {
  // put your main code here, to run repeatedly:
  howLongItsBeen = millis() - lastTimeItHappened;
  if(howLongItsBeen >= howLongToWait){
    if(myaddress == 2){
        lastTimeItHappened = millis();
        float y = 105.23;
        byte label = 2;
        byte src = 3;
        char s[7];
        //dtostrf(sourcefloat, tamanho minimo da string, nº de casas decimais, string destino) RPI pode ter que se mudar
        dtostrf(y, 6, 2,s);       
        i2c->sendToAll(label,s);
     }
  }
}