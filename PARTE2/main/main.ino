#include "commi2c.h"
#include <Wire.h>
#include <LinkedList.h>

const int ledPin = 9;
const int analogInPin = A0; // Analog input pin that the LDR is attached to

// Pin used to check if the arduino is the nº1 or nº2
const int idPin = 7;
// my i2c address
int myaddress = -1;

CommI2C i2c(A0, 9); 

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

	byte first8;
	byte last8;
  
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
  Serial.print("calib ended");

}

void loop() {

	i2c.checkFlags();
  
  
}
