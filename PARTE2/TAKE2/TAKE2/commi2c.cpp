#include "commi2c.h"	

//default constructor
CommI2C::CommI2C() {

	ldrPin = A0;
	ledPin = 9;

}

//constructor
CommI2C::CommI2C(int ldrPin_, int ledPin_) {
   int ldrPin = ldrPin_;
   int ledPin = ledPin_;
}

//set arduino's own address in the object
void CommI2C::setMyAddress (int address) {
   myAddress = address;
}

//decodes the message received from i2c
void CommI2C::msgDecoder(int label, int src_addr, String data){
   Serial.print("label = ");
   Serial.println(label);

   Serial.print("src = ");
   Serial.println(src_addr);

   Serial.print("data = ");
   Serial.println(data);

   float value = data.toFloat();
   Serial.print("value = ");
   Serial.println(value);
  
}

