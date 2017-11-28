void ReadADCCalib(){	

	ADC= analogRead(InPin); //reading ADC values

	//sending read ack
	Wire.beginTransmission(address);	
	Wire.write(first byte);	
	Wire.write(second byte);
	Wire.endTransmission();

}