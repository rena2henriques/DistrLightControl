#ifndef DATABASE_H
#define DATABASE_H

#include <cstdlib>
#include <iostream>
#include <string>
#include <ctime>
#include <chrono>
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

	struct Info {
		float data;
		std::chrono::system_clock::time_point timestamp; 
	};

	struct Buffer {
		// for illuminance values
		boost::circular_buffer<struct Info> ilum;
		// for duty cycle values
		boost::circular_buffer<struct Info> dutyCycle;

		int lastRead = 0;
	};

	// Create a circular buffer for floats
	Buffer buffs[127];

	// num of nodes for the T gets
	int numBuffers = 0;

	// time of the last restart 
	std::chrono::system_clock::time_point last_restart;

public:

	Database(int capacity);

	~Database();

	void insertBuffer(int address, float lux, float dc);

	void clearBuffers();

	std::string getCurrentValues(char message[]);

	std::string getLastMinuteValues(char message[]);

	// for the T gets
	int getNumBuffers();

	int getLastReadState(int address);

	void setLastReadState(int address);

	std::string getStreamValues(int address, char type);

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
	float timeSeconds = 0;

	// last sender
	int last_sender = -1;
};

#endif