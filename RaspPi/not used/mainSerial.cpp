#include "serialComm.h"

#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
using namespace boost::system;
using namespace boost::asio;

int main() {
	
	io_service ard_service;

	SerialComm arduino(ard_service, "/dev/ttyACM0");

	ard_service.run();

	return 0;
}