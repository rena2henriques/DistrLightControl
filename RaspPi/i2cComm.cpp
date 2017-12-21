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
				   
	printf("I2C has been initialised\n");
}

I2Comm::~I2Comm(){

	// Terminates the library
	gpioTerminate();
	db->~Database();
}

void I2Comm::sniffer() {

	printf("I'm sniffing\n");

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
	char message[30] = "";
	strncpy(message, msgBuf, size);

	switch (message[0]) {
				case 'g':
					// convert to int
					if( sscanf(message, "%c %d %f %f", &order, &address, &lux, &pwm) != 4)
						break; // message hasnt been sent correctly

					mtx.lock();
					db->insertBuffer(address, lux, pwm);
					mtx.unlock();

					break;
				case 'o':
					//printf("Entered 'o' case\n");
					// convert to int
					if( sscanf(message, "%c %d %f", &order, &address, &occup) != 3)
						break; // message hasnt been sent correctly

					mtx.lock();
					//inserts in the variable
					if (occup == 1){
						db->occupancy = 1;
					} else if (occup == 0) {
						db->occupancy = 0;
					}

					db->last_sender = address;

					// sets the flag to HIGH
					oFlag = 1;
					mtx.unlock();
					
					break;
				case 'L':
					// convert to float
					if( sscanf(message, "%c %d %f", &order, &address, &value) != 3)
						break; // message hasnt been sent correctly

					mtx.lock();
					//inserts in the variable
					db->ilumLowB = value;
					db->last_sender = address;

					// sets the flag to HIGH
					LFlag = 1;
					mtx.unlock();

					break;
				case 'O':
					// convert to float
					if (sscanf(message, "%c %d %f", &order, &address, &value) != 3)
						break; // message hasnt been sent correctly

					mtx.lock();
					//inserts in the variable
					db->extilum = value;

					db->last_sender = address;

					// sets the flag to HIGH
					OFlag = 1;

					mtx.unlock();

					break;
				case 'r':
					// convert to float
					if (sscanf(message, "%c %d %f", &order, &address, &value) != 3)
						break; // message hasnt been sent correctly

					mtx.lock();
					//inserts in the variable
					db->refilum = value;

					db->last_sender = address;

					// sets the flag to HIGH
					rFlag = 1;
					mtx.unlock();

					break;
				case 't':
					// convert to float
					if (sscanf(message, "%c %d %f", &order, &address, &value) != 3)
						break; // message hasnt been sent correctly

					mtx.lock();
					//inserts in the variable
					db->timeSeconds = value;

					db->last_sender = address;

					// sets the flag to HIGH
					tFlag = 1;

					mtx.unlock();

					break;

				case 'p':		
					// fazer para o caso de ser só um i <----------------

					if (message[2] != 'T') {
						// convert to float
						if (sscanf(message, "%c %d %f", &order, &address, &value) != 3)
							break; // message hasnt been sent correctly

						mtx.lock();
						//inserts in the variable
						db->instPow = value;

						db->last_sender = address;

						pFlag = 1;

						mtx.unlock();
						
					// fazer caso de ser o total 
					} else {
						if (sscanf(message, "%c %c %f", &order, &total, &value) != 3)
							break; // message hasnt been sent correctly

						mtx.lock();
						//inserts in the variable
						db->instPowT += value;
						mtx.unlock();
						

						// para detetar se as mensagens de todos os arduinos já foram recebidas
						numReceives++;

						if (db->getNumBuffers() == numReceives) {
							mtx.lock();
							pTFlag = 1;
							mtx.unlock();
							numReceives = 0;
						}
					}

					break;
				case 'e':
					// fazer para o caso de ser só um i
					if (message[2] != 'T') {
						if (sscanf(message, "%c %d %f", &order, &address, &value) != 3)
							break; // message hasnt been sent correctly

						mtx.lock();
						//inserts in the variable
						db->accumEn = value;

						db->last_sender = address;

						eFlag = 1;
						mtx.unlock();
						
					// fazer caso de ser o total 
					} else {
						if (sscanf(message, "%c %c %f", &order, &total, &value) != 3)
							break; // message hasnt been sent correctly

						mtx.lock();
						//inserts in the variable
						db->accumEnT += value;

						mtx.unlock();

						// para detetar se as mensagens de todos os arduinos já foram recebidas
						numReceives++;

						if (db->getNumBuffers() == numReceives) {
							mtx.lock();
							eTFlag = 1;
							mtx.unlock();
							numReceives = 0;
						}
					}
					
					break;
				case 'c':

					// fazer para o caso de ser só um i 
					if (message[2] != 'T') {
						// convert to float
						if (sscanf(message, "%c %d %f", &order, &address, &value) != 3)
							break; // message hasnt been sent correctly

						mtx.lock();
						//inserts in the variable
						db->accumConf = value;

						db->last_sender = address;

						cFlag = 1;
						mtx.unlock();
					// fazer caso de ser o total 
					} else {
						if (sscanf(message, "%c %c %f", &order, &total, &value) != 3)
							break; // message hasnt been sent correctly

						mtx.lock();
						//inserts in the variable
						db->accumConfT += value;

						mtx.unlock();

						// para detetar se as mensagens de todos os arduinos já foram recebidas
						numReceives++;

						if (db->getNumBuffers() == numReceives) {
							mtx.lock();
							cTFlag = 1;
							mtx.unlock();
							numReceives = 0;
						}
					}

					break;
				case 'v':

					// fazer para o caso de ser só um i 
					if (message[2] != 'T') {
						if (sscanf(message, "%c %d %f", &order, &address, &value) != 3)
							break; // message hasnt been sent correctly

						mtx.lock();
						//inserts in the variable
						db->accumVar = value;

						db->last_sender = address;
						
						vFlag = 1;
						mtx.unlock();

					// fazer caso de ser o total 
					} else {
						if (sscanf(message, "%c %c %f", &order, &total, &value) != 3)
							break; // message hasnt been sent correctly

						mtx.lock();
						//inserts in the variable
						db->accumVarT += value;

						mtx.unlock();

						// para detetar se as mensagens de todos os arduinos já foram recebidas
						numReceives++;

						if (db->getNumBuffers() == numReceives) {
							mtx.lock();
							vTFlag = 1;
							mtx.unlock();
							numReceives = 0;
						}
					}
					
					break;
	}

	return;
}

// deals with the flags that the other thread set to HIGH
std::string I2Comm::receiveGet() {

	char aux_response[30];

	// perceber se vale a pena verificar qual o request que eu estou à procura

	while(1) {

		mtx.lock();
		if( oFlag == 1  ){

			// converts
			snprintf(aux_response, 30, "o %d %d", db->last_sender, db->occupancy);

			oFlag = 0;
			break;			
		} else if (LFlag == 1) {
			// converts
			snprintf(aux_response, 30, "L %d %.2f", db->last_sender, db->ilumLowB);

			LFlag = 0;
			break;
		} else if (OFlag == 1) {
			// converts
			snprintf(aux_response, 30, "O %d %.2f", db->last_sender, db->extilum);

			OFlag = 0;
			break;
		} else if (rFlag == 1) {
			// converts
			snprintf(aux_response, 30, "r %d %.2f", db->last_sender, db->refilum);

			rFlag = 0;

			break;

		} else if (pFlag == 1) {

			snprintf(aux_response, 30, "p %d %.2f", db->last_sender, db->instPow);

			pFlag = 0;
			break;

		} else if (pTFlag == 1) {
			// converts
			snprintf(aux_response, 30, "p T %.2f", db->instPowT);

			db->instPowT = 0;

			pTFlag = 0;
			break;

		} else if (eFlag == 1) {
			// converts
			snprintf(aux_response, 30, "e %d %.2f", db->last_sender, db->accumEn);

			eFlag = 0;
			break;

		} else if (eTFlag == 1) {
			// converts
			snprintf(aux_response, 30, "e T %.2f", db->accumEnT);

			db->accumEnT = 0;
			eTFlag = 0;
			break;

		} else if (cFlag == 1) {
			snprintf(aux_response, 30, "c %d %.2f", db->last_sender, db->accumConf);

			cFlag = 0;
			break;

		} else if (cTFlag == 1) {
			// converts
			snprintf(aux_response, 30, "c T %.2f", db->accumConfT);

			db->accumConfT = 0;

			cTFlag = 0;
			break;

		} else if (vFlag == 1) {
			snprintf(aux_response, 30, "v %d %.2f", db->last_sender, db->accumVar);			

			vFlag = 0;
			break;

		} else if (vTFlag == 1) {
			// converts
			snprintf(aux_response, 30, "v T %.2f", db->accumVarT);

			db->accumVarT = 0;

			vTFlag = 0;
			break;
		} else if (tFlag == 1) {
			snprintf(aux_response, 30, "t %d %.2f", db->last_sender, db->timeSeconds);

			tFlag = 0;
			break;
		}

		mtx.unlock();
		
	}
	mtx.unlock();

	std::string response(aux_response);

	return response;
}