#ifndef DATABASE_H
#define DATABASE_H

#include <boost/circular_buffer.hpp>

class Database {

private:

// Create a circular buffer for floats
// for illuminance values
boost::circular_buffer<float> ilum;
// for duty cycle values
boost::circular_buffer<float> dutyCycle;
// for duty cycle values
boost::circular_buffer<float> timestamp;

public:



};

#endif