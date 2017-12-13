#include "i2cComm.h"

I2Comm::I2Comm(shared_ptr <Database> db_) : db(db_){

	// Initialises the library
	if (gpioInitialise() < 0) {
	  printf("Error: GPIO Library failed to initialise\n");
	}

	/* Initialize Slave*/
	gpioSetMode(18, PI_ALT3);
	gpioSetMode(19, PI_ALT3);	
	xfer.control = (0x48<<16) | /* Slave address <------------*/
				   (0x00<<13) | /* invert transmit status flags */
				   (0x00<<12) | /* enable host control */
				   (0x00<<11) | /* enable test fifo */
				   (0x00<<10) | /* inverte receive status flags */
				   (0x01<<9) | /* enable receive */
				   (0x00<<8) | /* enable transmit */
				   (0x00<<7) | /* abort operation and clear FIFOs */
				   (0x00<<6) | /* send control register as first I2C byte */
				   (0x00<<5) | /* send status register as first I2C byte */
				   (0x00<<4) | /* set SPI polarity high */
				   (0x00<<3) | /* set SPI phase high */
				   (0x01<<2) | /* enable I2C mode */
				   (0x00<<1) | /* enable SPI mode */
				   0x01 ;      /* enable BSC peripheral */
				   
}


I2Comm::~I2Comm(){

	// Terminates the library
	gpioTerminate();
	
}

void I2Comm::sniffer() {

	//int j = 0;

	while(end != 1){
		usleep(1000);

		status = bscXfer(&xfer);
		if ( xfer.rxCnt != 0 ){
			printf("Received %d bytes\n", xfer.rxCnt);
			printf("%.*s\n", xfer.rxCnt, xfer.rxBuf);

	        /*for(j=0;j<xfer.rxCnt;j++)
			    printf("%c",xfer.rxBuf[j]);
		    printf("\n");*/

		   	db->insertBuffer(1, 5.5, 0.1);
		}

		//memset(xfer.rxBuf, 0, xfer.rxCnt);
	}
}

// processes the data received
void I2Comm::readData(char message[]) {

	if (message[0] == 'o'){ // pensar no protocolo de comunicação
		printf("nothing happens\n");
	}

	return;
}


std::string I2Comm::receiveGet(char request) {

	std::lock_guard<std::mutex> lock(mtx);

	while(1){
		status = bscXfer(&xfer);
		if ( xfer.rxCnt != 0 ){
			printf("Received %d bytes\n", xfer.rxCnt);
			printf("%.*s\n", xfer.rxCnt, xfer.rxBuf);

			if (request == 'o') {
				break;
			}  
		}
	}

	std::string response(xfer.rxBuf);
	response.append('\n');

	return response;
}



