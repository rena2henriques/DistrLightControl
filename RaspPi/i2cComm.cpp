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

			// processes data
			readData(xfer.rxBuf, (int) xfer.rxCnt);
		}
	}
}

// processes the data received
void I2Comm::readData(char msgBuf[], int size) {

	// fazer um strcpy para só ter a parte importante da message
	char message[30];
	strncpy(message, msgBuf, size);

	switch (message[0]) {
				case 'g':
					// convert to int
					if( sscanf(message, "%c %d %f %f", &order, &address, &lux, &pwm) != 4)
						break; // message hasnt been sent correctly

					db->insertBuffer(address, lux, pwm);

					break;
				case 'o':
					// convert to int
					if( sscanf(message, "%c %d %d", &order, &address, &occup) != 3)
						break; // message hasnt been sent correctly

					mtx.lock();
					//inserts in the variable
					if (occup == 1){
						db->occupancy = 1;
					} else if (occup == 0) {
						db->occupancy = 0;
					}

					db->last_sender = address;
					mtx.unlock();

					// sets the flag to HIGH
					oFlag = 1;
					break;
				case 'L':
					// convert to float
					if( sscanf(message, "%c %d %f", &order, &address, &value) != 3)
						break; // message hasnt been sent correctly

					mtx.lock();
					//inserts in the variable
					db->ilumLowB = value;

					db->last_sender = address;
					mtx.unlock();

					// sets the flag to HIGH
					LFlag = 1;
					break;
				case 'O':
					// convert to float
					if (sscanf(message, "%c %d %f", &order, &address, &value) != 3)
						break; // message hasnt been sent correctly

					mtx.lock();
					//inserts in the variable
					db->extilum = value;

					db->last_sender = address;
					mtx.unlock();

					// sets the flag to HIGH
					OFlag = 1;
					break;
				case 'r':
					// convert to float
					if (sscanf(message, "%c %d %f", &order, &address, &value) != 3)
						break; // message hasnt been sent correctly

					mtx.lock();
					//inserts in the variable
					db->refilum = value;

					db->last_sender = address;
					mtx.unlock();

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
					// fazer para o caso de ser só um i

					// fazer caso de ser o total

					vFlag = 1;
					break;
			}



	return;
}


std::string I2Comm::receiveGet() {

	char aux_response[20];

	// perceber se vale a pena verificar qual o request que eu estou à procura

	while(1){

		if( oFlag == 1 ){
			// converts
			snprintf(aux_response, 20, "o %d %d", db->last_sender, db->occupancy);

			oFlag = 0;
			break;			
		} else if (LFlag == 1) {
			// converts
			snprintf(aux_response, 20, "L %d %f", db->last_sender, db->ilumLowB);

			LFlag = 0;
			break;
		} else if (OFlag == 1) {
			// converts
			snprintf(aux_response, 20, "O %d %f", db->last_sender, db->extilum);

			OFlag = 0;
			break;
		} else if (rFlag == 1) {
			// converts
			snprintf(aux_response, 20, "r %d %f", db->last_sender, db->refilum);

			rFlag = 0;
			break;

		} else if (pFlag == 1) {

			pFlag = 0;
			break;

		} else if (pTFlag == 1) {


			pTFlag = 0;
			break;

		} else if (eFlag == 1) {

			eFlag = 0;
			break;

		} else if (eTFlag == 1) {

			eTFlag = 0;
			break;

		} else if (cFlag == 1) {

			cFlag = 0;
			break;

		} else if (cTFlag == 1) {

			cTFlag = 0;
			break;

		} else if (vFlag == 1) {

			vFlag = 0;
			break;

		} else if (vTFlag == 1) {

			vTFlag = 0;
			break;
		}
		
	}

	std::string response(aux_response);

	return response;
}