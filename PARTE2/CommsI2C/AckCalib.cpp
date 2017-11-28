void AckCalib(){
	
	n_ack++;

	//checking if all nodes have already read the lux values
	if(n_ack==n_nodes){
		n_ack=0; //in case another calibration is needed
		analogWrite(OutPin, LOW); //turn off led

		//finding the next node
		codigo para encontrar o node seguinte, depende de como isto estiver feito

		//notifies next node that is his turn
		Wire.beginTransmission(nextnode_add);	
		Wire.write(first byte);	
		Wire.write(second byte);
		Wire.endTransmission();

	}	

}

