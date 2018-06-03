#ifndef I2C_COMM
#define I2C_COMM

#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <pigpio.h>
#include <memory.h>
#include <mutex>
#include "database.h"
#include <boost/algorithm/string/split.hpp>
//#include <boost/algorithm/string/classification.hpp>
#include <vector>

class I2Comm {
private:

	// Slave address = 0x48
	bsc_xfer_t xfer;

	// return of the read function 
	int status;

	// tests if the end of the sniffer has arrived
	int end = 0;

	// creats the object where we handle the data coming from arduino
	shared_ptr <Database> db;

	std::mutex mtx;

	// Flags for i2c loop - not pretty
	int oFlag = 0; // 3º
	int LFlag = 0; // 4º
	int OFlag = 0; // 5º
	int rFlag = 0; // 6º
	int pFlag = 0; // 7º
	int pTFlag = 0; // 8º
	int eFlag = 0; // 9º
	int eTFlag = 0; // 10º
	int cFlag = 0; // 11º
	int cTFlag = 0; // 12º
	int vFlag = 0; // 13º
	int vTFlag = 0; // 14º
	int tFlag = 0; // 15º time since last restart

	// aux variables
	char order = 'z';
	char total = 'z';
	int address = -1;
	float occup = 0;
	float value = 0.0;
	float lux = 0.0;
	float pwm = 0.0;
	int numReceives = 0;

public:

	I2Comm(shared_ptr <Database> db_);

	~I2Comm();

	// receives data from arduinos
	void sniffer();

	// processes the data coming from the arduinos
	void readData(char message[], int size);

	std::string receiveGet();
};

#endif