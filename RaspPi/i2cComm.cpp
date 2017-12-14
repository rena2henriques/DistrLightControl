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

	while(end != 1){
		usleep(5000);

		status = bscXfer(&xfer);
		if ( xfer.rxCnt != 0 ){
			printf("Received %d bytes\n", xfer.rxCnt);
			printf("%.*s\n", xfer.rxCnt, xfer.rxBuf);

			//std::string message(xfer.rxBuf, xfer.rxCnt);

			switch (xfer.rxBuf[0]) {
				case 'g':
					break;
				case 'o':
					//inserts in the variable

					// sets the flag to HIGH
					oFlag = 1;
					break;
				case 'L':
					//inserts in the variable
					
					// sets the flag to HIGH
					LFlag = 1;
					break;
				case 'O':
					//inserts in the variable
					
					// sets the flag to HIGH
					OFlag = 1;
					break;
				case 'r':
					//inserts in the variable
					
					// sets the flag to HIGH
					rFlag = 1;
					break;
				case 'p':
					
					// fazer para o caso de ser só um i

					// fazer caso de ser o total
					pFlag = 1;
					break;
				case 'e':
					// fazer para o caso de ser só um i

					// fazer caso de ser o total
					eFlag = 1;
					break;
				case 'c':
					// fazer para o caso de ser só um i

					// fazer caso de ser o total
					cFlag = 1;
					break;
				case 'v':
					vFlag = 1;
					break;
				default:
					//do nothins
			}

		   	
		   	//readData(message);
		}
	}
}

// processes the data received
void I2Comm::readData(std::string message) {

	using namespace boost::algorithm;

	std::vector<std::string> tokens;

    split(tokens, message, is_any_of(" ")); // here it is

	if (tokens[0] == "g"){ // pensar no protocolo de comunicação
		printf("Inserted in buffer\n");

		db->insertBuffer(std::stoi(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
	}

	return;
}


std::string I2Comm::receiveGet(char request) {

	//std::lock_guard<std::mutex> lock(mtx);

	while(1){

		if( goFlag == 1 ){
			
		} else if (gLFlag == 1) {


		} else if (gOFlag == 1) {


		} else if (grFlag == 1) {


		} else if (gpFlag == 1) {


		} else if (gpTFlag == 1) {


		} else if (geFlag == 1) {


		} else if (geTFlag == 1) {


		} else if (gcFlag == 1) {


		} else if (gcTFlag == 1) {


		} else if (gvFlag == 1) {


		} else if (gvTFlag == 1) {


		}
		
	}


	return response;
}


// dar reset às flags