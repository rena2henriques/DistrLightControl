#ifndef DATABASE_H
#define DATABASE_H

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <boost/circular_buffer.hpp>
using namespace std;

class Database {

private:
// to define the capacity of the buffers
int capacity;

struct Buffer {
	// for illuminance values
	boost::circular_buffer<float> ilum;
	// for duty cycle values
	boost::circular_buffer<float> dutyCycle;
};

// Create a circular buffer for floats
Buffer buffs[127];

//boost::circular_buffer<float> timestamp; // not needed for now

public:

	Database();

	Database(int capacity);

	~Database();

	void insertBuffer(int address, float lux, float dc);

	void clearBuffers();

	void printBuffers(int address);

	std::string getCurrentValues(char message[]);

	// temp variables to save the info
	int occupancy = 0;
	float ilumLowB = 0;
	float extilum = 0;
	float refilum = 0;
	float instPow = 0;
	float instPowT = 0;
	float accumEn = 0;
	float accumEnT = 0;
	float accumConf = 0;
	float accumConfT = 0;
	float accumVar = 0;
	float accumVarT = 0;

	// last sender
	int last_sender = -1;
};

#endif