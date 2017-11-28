void HighCalib(){

	analogWrite(OutPin, HIGH); //pwm to high <-- ver se é mesmo high ou se é outra ref

	delay(tempo); //um tempo maior que aquilo demora a estabilizar

	ADC= analogRead(InPin); //reads its own value

	//sends message for all arduinos to read their lux values
	for(i=0,i++;i<n_nodes){
		Wire.beginTransmission(address(i));	//algo assim
		Wire.write(c);	//first byte
		Wire.write(c);	//second byte
		Wire.endTransmission();
	}

	//reads its own values

}