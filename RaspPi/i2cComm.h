#ifndef I2C_COMM
#define I2C_COMM

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <pigpio.h>
#include <memory.h>

class I2Comm {

private:

	// Slave address = 0x48
	bsc_xfer_t xfer;

	// return of the read function 
	int status;

	// tests if the end of the sniffer has arrived
	int end = 0;


public:

I2Comm();

~I2Comm();

// receives data from arduinos
void sniffer();

// processes the data coming from the arduinos
void readData(char message[]);

};

#endif