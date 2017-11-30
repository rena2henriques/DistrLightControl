#include "commi2c.h"	

CommI2C::CommI2C() {

	ldrPin = A0;
	ledPin = 9;

	n_ack = 0;
	myaddress = 0;

}

CommI2C::CommI2C(int ldrPin_, int ledPin_) {

	ldrPin = ldrPin_;
	n_ack = 0;

	ledPin = ledPin_;
	myaddress = 0;

}

void CommI2C::setAddress(int address) {
	myaddress = address;
}

int CommI2C::findNodes() {

	// broadcast address is 0

	unsigned char error, address;

	// the devices have 7-bit I2C addresses 
	for(address = 1; address<127;address++){

	    // We use the Write.endTransmisstion return value to see if
	    // a device did acknowledge to the address.
	    Wire.beginTransmission(address);
	    error = Wire.endTransmission();

	    // The data was send successfully
	    if (error == 0) {
	    	// Inserts the discovered address in the list
			addrList.add(address);
	    }
	}
	
	return addrList.size();
}


// returns the address in the index i
unsigned char CommI2C::getAddr(int i) {
	return addrList.get(i);
}

void CommI2C::calibration(int myaddress) {

	// the arduino that begins the calibration is the 1
	if (myaddress == 1) {

		// starts the calibration proccess
		ledON();

	} 

	// runs until we read every led lux including ourself
	while(it != addrList.size() + 1 ) {
		delay(10);
	}

	// NÂO ESQUECER QUE ELE TEM QUE LER OS O

	return;
}




// decodes the message in label and data
void CommI2C::msgDecoder(byte last8, byte first8){
  
    byte label = last8 >> 2;
    byte two_bits = last8 & 3;
      
    Serial.print("label="); // work
    Serial.println(label);

    Serial.print("two_bits="); // work
    Serial.println(two_bits);

    // 512 because the power starts at 0!!!!
	int value = 512*(two_bits >> 1) + 256*(two_bits & 1) + first8;

	if (label == 1) {
		// reads the lux value from ldr and ACKs
		readADC(value);
	// } else if (label == 2) {
	// 	// Checks if the arduino can turn the led off
	// 	checkTurnEnd();
	// } else if (label == 3) {
 //    ledON();

	}

}

// reads the LUX Value at the moment
void CommI2C::readADC(int address) {

	Serial.println("enter"); // working


	ADC = analogRead(ldrPin);

	// Juntar a um vetor e converter para lux

	// temp - working
	Serial.print("ADC=");
	Serial.println(ADC);

	it++;

	// NOT WORKING

	// Wire.beginTransmission((byte) address);
	// // Wire.write((byte) 8); //sends 00001000
	// // Wire.write((byte) 0); //XXXXXX no data needed
	// Wire.endTransmission();
}

// checks if their time of LED High has ended
void CommI2C::checkTurnEnd() {

	int nextaddress;

	n_ack++;

	if(n_ack == addrList.size()) {
		// resets n_ack flag
		n_ack = 0;

		// turns off led
		analogWrite(ledPin, LOW);

		// iterate through address list
		for(int i = 0; i < addrList.size(); i++){
			// até encontrar a address imediatamente a seguir à minha
			if (addrList.get(i) > myaddress) {
				nextaddress = addrList.get(i);
				break;
			}	
		}

		Wire.beginTransmission(nextaddress);
		Wire.write(12); // sends 1100
		Wire.write(0); // no data needed
		Wire.endTransmission();
	}

	return;
}


void CommI2C::ledON(){

	// turns led ON
	analogWrite(ledPin, HIGH);

	// waits for the system to stabilize
	delay(30);

	// reads the lux value of its own led
	ADC = analogRead(ldrPin);

	// temp
	it++;

	// test -------------------------
	Serial.print("my addr = ");
	Serial.println(myaddress);

	// ------------------------------


	// sends message to all arduinos to read their lux values
	for(int i=0; i < addrList.size(); i++) {
		Wire.beginTransmission(addrList.get(i));
		Wire.write((byte) 7); // sends 0100
		Wire.write(myaddress); // no data needed
		Wire.endTransmission();
	}

}
