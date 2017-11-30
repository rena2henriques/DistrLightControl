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
	while(n_reads != addrList.size() + 1 ) {

		if(sendAck != 0){
			Serial.println("sendAck");

			// tells the HIGH node that it has read
			send((byte) sendAck, (byte) 8, (byte) 0);

			sendAck = 0;
		} else if (turnEnd != 0){

			Serial.println("sendAck");

			// tells the HIGH node that it has read
			send((byte) turnEnd, (byte) 12, (byte) 0);

			turnEnd = 0;
		} else if (ledFlag != 0) {

			Serial.println("ledFlag");
			
			// sends message to all arduinos to read their lux values
			for(int i=0; i < addrList.size(); i++) {

				// requests lux reading
				send((byte) addrList.get(i), (byte) 7, (byte) myaddress);
			}
		}


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

	Serial.print("value="); // work
    Serial.println(value);

	if (label == 1) {
		// reads the lux value from ldr and ACKs
		readADC(value);
	} else if (label == 2) {
		// Checks if the arduino can turn the led off
		checkTurnEnd();
	} else if (label == 3) {
		// sets the LED ON and asks for reading
 	   	ledON();
	}

}

// reads the LUX Value at the moment
void CommI2C::readADC(int address) {

	// Reads from LDR
	ADC = analogRead(ldrPin);

	// Juntar a um vetor e converter para lux <-----------------

	// temp - working
	Serial.print("ADC=");
	Serial.println(ADC);
	// -------------------------------------

	// The number of columns of my K matrix
	n_reads++;

	sendAck = address;
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

		turnEnd = nextaddress;
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

	// The number of columns of my K matrix
	n_reads++;


	ledFlag = 1;

}


// used just to simplifying the proccess of sending messages
byte CommI2C::send(byte address, byte firstByte, byte secondByte) {

	Wire.beginTransmission(address);
	Wire.write(firstByte);
	Wire.write(secondByte);
	return Wire.endTransmission();
}