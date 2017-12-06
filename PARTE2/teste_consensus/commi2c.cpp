#include "commi2c.h"	

CommI2C::CommI2C() {

	ldrPin = A0;
	ledPin = 9;

	n_ack = 0;
	myaddress = 0;

  //just to initialize the 2 first positions for Consensus
  dList.add(0);
  dList.add(0);
  
  consensusFlag=0;
}

CommI2C::CommI2C(int ldrPin_, int ledPin_) {

	ldrPin = ldrPin_;
	n_ack = 0;

	ledPin = ledPin_;
	myaddress = 0;

  //just to initialize the 2 first positions for Consensus
  dList.add(0);
  dList.add(0);

  consensusFlag=0;
}

void CommI2C::setAddress(int address) {
	myaddress = address;
}

int CommI2C::findNodes() {

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

void CommI2C::calibration() {

	// Resets flags
	sendAck = 0;
	turnEnd = 0;
	ledFlag = 0;
	calibFlag = 0;
	n_reads = 0;

	// the arduino that begins the calibration is the 1
	if (myaddress == 1) {

		// starts the calibration proccess
		ledON();
	} 

	// runs until we read every led lux including ourself
	while(n_reads != addrList.size() + 1 ) {

		if(sendAck != 0){

			// read another one
      		n_reads++;
     		
			// tells the HIGH node that it has read
			send((byte) sendAck, (byte) 8, (byte) 0);
    
			sendAck = 0;
		} else if (turnEnd != 0){

 			// read my own
     		n_reads++;

     		Serial.print("n_reads=");
			Serial.println(n_reads);
     
			// tells the next high node to light it up
			send((byte) turnEnd, (byte) 12, (byte) 0);
      
			turnEnd = 0;

		} else if (ledFlag != 0) {

		    //wait for the system to stabilize
		    delay(100);
		
			sendToAll((byte) 7, (byte) myaddress);

			ledFlag = 0;

		} else if (calibFlag == 1) {

			// clears the list
			addrList.clear();
			// find nodes again
			findNodes();
			// recalibration	
			calibration();	
		}

	}

	// temp
	analogWrite(ledPin, LOW);

	Serial.println("calib ended");

	//-------mudar este delay - ver qual o melhor
	delay(50); //waits until all leds are turned off

	//reads background illumination
	ADCList.add(analogRead(ldrPin));


	return;
}




// decodes the message in label and data
void CommI2C::msgDecoder(byte last8, byte first8){
  
    byte label = last8 >> 2;
    byte two_bits = last8 & 3;
      
    // 512 because the power starts at 0!!!!
	int value = 512*(two_bits >> 1) + 256*(two_bits & 1) + first8;

	Serial.print("label=");
	Serial.println(label);

	if (label == 1) {
		// reads the lux value from ldr and ACKs
		readADC(value);
	} else if (label == 2) {
		// Checks if the arduino can turn the led off
		checkTurnEnd();
	} else if (label == 3) {
		// sets the LED ON and asks for reading
 	   	ledON(); 
	} else if (label == 4) {
		// recalibrate
		Serial.println("calibFlag ON");

		calibFlag = 1;
	} else if (label==5){

		dList.set(twod_flag, value);
		twod_flag++;

		if(twod_flag==2){
			consensusFlag=1;
			twod_flag=0;
          
		}
	}

}

// reads the LUX Value at the moment
void CommI2C::readADC(int address) {

	// Reads from LDR
	ADCList.add(analogRead(ldrPin));

	// Juntar a um vetor e converter para lux <-----------------

	// temp - working
	Serial.print("ADC=");
	Serial.println(ADC);
	// -------------------------------------


	sendAck = address;
}

// checks if their time of LED High has ended
void CommI2C::checkTurnEnd() {
  
	int nextaddress;
  
	n_ack++;

	if(n_ack == addrList.size() && n_reads != addrList.size() + 1) {

		//read my own value
  		ADCList.add(analogRead(ldrPin));

  		// temp - working
		Serial.print("ADC=");
		Serial.println(ADC);

		// resets n_ack flag
		n_ack = 0;

    	int im_last_one = 1;
		// turns off led
		analogWrite(ledPin, LOW);

		// iterate through address list
		for(int i = 0; i < addrList.size(); i++){
			// até encontrar a address imediatamente a seguir à minha
			if (addrList.get(i) > myaddress) {
				nextaddress = addrList.get(i);
        		im_last_one = 0;
				break;
			}	
		}

	    if(im_last_one == 1)
	    	turnEnd = 100; //100 is irrelevant, just to make sure nextaddr not 0
		else
			turnEnd = nextaddress; //not last node
 
	}

	return;
}


void CommI2C::ledON(){

	// turns led ON
	analogWrite(ledPin, HIGH);

	ledFlag = 1;

}


// used just to simplifying the proccess of sending messages
byte CommI2C::send(byte address, byte firstByte, byte secondByte) {

	Wire.beginTransmission(address);
	Wire.write(firstByte);
	Wire.write(secondByte);
	return Wire.endTransmission();
}


void CommI2C::sendToAll(byte firstByte, byte secondByte) {

	// sends message to all arduinos to read their lux values
	for(int i=0; i < addrList.size(); i++) {
		// requests lux reading
		send((byte) addrList.get(i), (byte) firstByte, (byte) secondByte);
	}
}

// checks flags during to loop of the main program
void CommI2C::checkFlags() {

	if (calibFlag == 1){
		Serial.println("Recalib");

		// clears the list
		addrList.clear();
		// find nodes again
		findNodes();
		// recalibration
		calibration();
    reconsensusFlag=1;
	}

	return;
}


LinkedList<float> CommI2C::getADCvalues(){
	return ADCList;
}

int CommI2C::getConsensusFlag(){
  if(consensusFlag!=0)
  {
  Serial.print("consensus flag=");
  Serial.println(consensusFlag);
  }
  return consensusFlag;
}

