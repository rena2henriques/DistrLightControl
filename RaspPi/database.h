#ifndef DATABASE_H
#define DATABASE_H

#include <cstdlib>
#include <iostream>
#include <string>
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
};

#endif